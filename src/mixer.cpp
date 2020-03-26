/*
 *  (C) 2019-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include "clownaudio/mixer.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "decoding/decoders/common.h"

#include "decoding/decoder_selector.h"
#include "decoding/resampled_decoder.h"
#include "decoding/split_decoder.h"

#define CHANNEL_COUNT 2

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min, max) MIN(MAX((x), (min)), (max))

typedef struct Channel
{
	struct Channel *next;

	bool paused;
	bool free_when_done;
	float volume_left;
	float volume_right;
	DecoderStage *pipeline;
	void *resampled_decoder;
	ClownAudio_Sound sound;

	unsigned long fade_out_counter_max;
	unsigned long fade_in_counter_max;
	unsigned long fade_counter;
} Channel;

typedef struct Mutex
{
#ifdef _WIN32
	HANDLE handle;
#else
	pthread_mutex_t pthread_mutex;
#endif
} Mutex;

struct ClownAudio_Mixer
{
	Channel *channel_list_head;
	Mutex mutex;
	unsigned long sample_rate;
	ClownAudio_Sound sound_allocator;
};

struct ClownAudio_SoundData
{
	DecoderSelectorData *decoder_selector_data[2];
	unsigned char *file_buffers[2];
};

static bool LoadFileToMemory(const char *path, unsigned char **buffer, size_t *size)
{
	bool success = false;

	if (path == NULL)
	{
		*buffer = NULL;
		*size = 0;
		success = true;
	}
	else
	{
		FILE *file = fopen(path, "rb");

		if (file != NULL)
		{
			fseek(file, 0, SEEK_END);
			*size = ftell(file);
			rewind(file);

			*buffer = (unsigned char*)malloc(*size);

			if (*buffer != NULL)
			{
				fread(*buffer, 1, *size, file);

				success = true;
			}

			fclose(file);
		}
	}

	return success;
}

static void MutexInit(Mutex *mutex)
{
#ifdef _WIN32
	mutex->handle = CreateEventA(NULL, FALSE, TRUE, NULL);
#else
	pthread_mutex_init(&mutex->pthread_mutex, NULL);
#endif
}

static void MutexDeinit(Mutex *mutex)
{
#ifdef _WIN32
	CloseHandle(mutex->handle);
#else
	pthread_mutex_destroy(&mutex->pthread_mutex);
#endif
}

static void MutexLock(Mutex *mutex)
{
#ifdef _WIN32
	WaitForSingleObject(mutex->handle, INFINITE);
#else
	pthread_mutex_lock(&mutex->pthread_mutex);
#endif
}

static void MutexUnlock(Mutex *mutex)
{
#ifdef _WIN32
	SetEvent(mutex->handle);
#else
	pthread_mutex_unlock(&mutex->pthread_mutex);
#endif
}

static Channel* FindChannel(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	for (Channel *channel = mixer->channel_list_head; channel != NULL; channel = channel->next)
		if (channel->sound == sound)
			return channel;

	return NULL;
}

CLOWNAUDIO_EXPORT void ClownAudio_InitSoundDataConfig(ClownAudio_SoundDataConfig *config)
{
	config->predecode = false;
	config->must_predecode = false;
	config->dynamic_sample_rate = false;
}

CLOWNAUDIO_EXPORT void ClownAudio_InitSoundConfig(ClownAudio_SoundConfig *config)
{
	config->loop = false;
	config->do_not_free_when_done = false;
	config->dynamic_sample_rate = false;
}

CLOWNAUDIO_EXPORT ClownAudio_Mixer* ClownAudio_CreateMixer(unsigned long sample_rate)
{
	ClownAudio_Mixer *mixer = (ClownAudio_Mixer*)malloc(sizeof(ClownAudio_Mixer));

	if (mixer != NULL)
	{
		mixer->channel_list_head = NULL;

		mixer->sample_rate = sample_rate;

		mixer->sound_allocator = 0;

		MutexInit(&mixer->mutex);
	}

	return mixer;
}

CLOWNAUDIO_EXPORT void ClownAudio_DestroyMixer(ClownAudio_Mixer *mixer)
{
	MutexDeinit(&mixer->mutex);

	free(mixer);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromMemory(ClownAudio_Mixer *mixer, const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	ClownAudio_SoundData *sound_data = (ClownAudio_SoundData*)malloc(sizeof(ClownAudio_SoundData));

	if (sound_data != NULL)
	{
		DecoderSpec wanted_spec;

		wanted_spec.sample_rate = config->dynamic_sample_rate ? 0 : mixer->sample_rate;	// Do not change the sample rate when dynamic resampling is enabled
		wanted_spec.channel_count = CHANNEL_COUNT;
		wanted_spec.format = DECODER_FORMAT_F32;

		if (file_buffer1 != NULL && file_buffer2 != NULL)
		{
			sound_data->decoder_selector_data[0] = DecoderSelector_LoadData(file_buffer1, file_size1, config->predecode, config->must_predecode, &wanted_spec);
			sound_data->decoder_selector_data[1] = DecoderSelector_LoadData(file_buffer2, file_size2, config->predecode, config->must_predecode, &wanted_spec);

			if (sound_data->decoder_selector_data[0] != NULL && sound_data->decoder_selector_data[1] != NULL)
				return sound_data;

			DecoderSelector_UnloadData(sound_data->decoder_selector_data[1]);
			DecoderSelector_UnloadData(sound_data->decoder_selector_data[0]);
		}
		else if (file_buffer1 != NULL)
		{
			sound_data->decoder_selector_data[0] = DecoderSelector_LoadData(file_buffer1, file_size1, config->predecode, config->must_predecode, &wanted_spec);
			sound_data->decoder_selector_data[1] = NULL;

			if (sound_data->decoder_selector_data[0] != NULL)
				return sound_data;
		}
		else if (file_buffer2 != NULL)
		{
			sound_data->decoder_selector_data[0] = NULL;
			sound_data->decoder_selector_data[1] = DecoderSelector_LoadData(file_buffer2, file_size2, config->predecode, config->must_predecode, &wanted_spec);

			if (sound_data->decoder_selector_data[1] != NULL)
					return sound_data;
		}
	}

	return NULL;
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromFiles(ClownAudio_Mixer *mixer, const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config)
{
	if (intro_path != NULL || loop_path != NULL)
	{
		unsigned char *file_buffers[2];
		size_t file_buffer_sizes[2];

		if (LoadFileToMemory(intro_path, &file_buffers[0], &file_buffer_sizes[0]))
		{
			if (LoadFileToMemory(loop_path, &file_buffers[1], &file_buffer_sizes[1]))
			{
				ClownAudio_SoundData *sound_data = ClownAudio_LoadSoundDataFromMemory(mixer, file_buffers[0], file_buffer_sizes[0], file_buffers[1], file_buffer_sizes[1], config);

				if (sound_data != NULL)
				{
					sound_data->file_buffers[0] = file_buffers[0];
					sound_data->file_buffers[1] = file_buffers[1];

					return sound_data;
				}

				free(file_buffers[1]);
			}

			free(file_buffers[0]);
		}
	}

	return NULL;
}

CLOWNAUDIO_EXPORT void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound_data)
{
	if (sound_data != NULL)
	{
		if (sound_data->decoder_selector_data[0] != NULL)
			DecoderSelector_UnloadData(sound_data->decoder_selector_data[0]);

		if (sound_data->decoder_selector_data[1] != NULL)
			DecoderSelector_UnloadData(sound_data->decoder_selector_data[1]);

		free(sound_data->file_buffers[0]);
		free(sound_data->file_buffers[1]);
	}
}

CLOWNAUDIO_EXPORT ClownAudio_Sound ClownAudio_CreateSound(ClownAudio_Mixer *mixer, ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config)
{
	ClownAudio_Sound sound = 0;

	if (sound_data != NULL)
	{
		DecoderSpec wanted_spec;
		wanted_spec.sample_rate = mixer->sample_rate;
		wanted_spec.channel_count = CHANNEL_COUNT;
		wanted_spec.format = DECODER_FORMAT_F32;

		// Begin constructing the decoder pipeline

		DecoderStage *stage = (DecoderStage*)malloc(sizeof(DecoderStage));

		if (stage == NULL)
			return 0;

		// Let's start with the decoder-selectors

		DecoderStage *selector_stages[2];

		selector_stages[0] = (DecoderStage*)malloc(sizeof(DecoderStage));

		if (selector_stages[0] == NULL)
		{
			free(stage);
			return 0;
		}

		selector_stages[1] = (DecoderStage*)malloc(sizeof(DecoderStage));

		if (selector_stages[1] == NULL)
		{
			free(selector_stages[0]);
			free(stage);
			return 0;
		}

		void *decoder_selectors[2] = {NULL, NULL};
		DecoderSpec specs[2];

		if (sound_data->decoder_selector_data[0] != NULL)
		{
			decoder_selectors[0] = DecoderSelector_Create(sound_data->decoder_selector_data[0], sound_data->decoder_selector_data[1] != NULL ? false : config->loop, &wanted_spec, &specs[0]);

			if (decoder_selectors[0] != NULL)
			{
				selector_stages[0]->decoder = decoder_selectors[0];
				selector_stages[0]->Destroy = DecoderSelector_Destroy;
				selector_stages[0]->Rewind = DecoderSelector_Rewind;
				selector_stages[0]->GetSamples = DecoderSelector_GetSamples;
				selector_stages[0]->SetLoop = DecoderSelector_SetLoop;
			}
		}

		if (sound_data->decoder_selector_data[1] != NULL)
		{
			decoder_selectors[1] = DecoderSelector_Create(sound_data->decoder_selector_data[1], config->loop, &wanted_spec, &specs[1]);

			if (decoder_selectors[1] != NULL)
			{
				selector_stages[1]->decoder = decoder_selectors[1];
				selector_stages[1]->Destroy = DecoderSelector_Destroy;
				selector_stages[1]->Rewind = DecoderSelector_Rewind;
				selector_stages[1]->GetSamples = DecoderSelector_GetSamples;
				selector_stages[1]->SetLoop = DecoderSelector_SetLoop;
			}
		}

		if (decoder_selectors[0] == NULL && decoder_selectors[1] == NULL)
		{
			free(selector_stages[0]);
			free(selector_stages[1]);
			free(stage);
			return 0;
		}

		// Now for the split-decoder, if needed

		void *split_decoder = NULL;

		if (decoder_selectors[0] != NULL && decoder_selectors[1] != NULL)
		{
			if (specs[0].sample_rate != specs[1].sample_rate || specs[0].channel_count != specs[1].channel_count || specs[0].format != specs[1].format)
			{
				DecoderSelector_Destroy(decoder_selectors[0]);
				DecoderSelector_Destroy(decoder_selectors[1]);

				free(selector_stages[0]);
				free(selector_stages[1]);
				free(stage);
				return 0;
			}

			split_decoder = SplitDecoder_Create(selector_stages[0], selector_stages[1]);

			if (split_decoder == NULL)
			{
				DecoderSelector_Destroy(decoder_selectors[0]);
				DecoderSelector_Destroy(decoder_selectors[1]);

				free(selector_stages[0]);
				free(selector_stages[1]);
				free(stage);
				return 0;
			}

			stage->decoder = split_decoder;
			stage->Destroy = SplitDecoder_Destroy;
			stage->Rewind = SplitDecoder_Rewind;
			stage->GetSamples = SplitDecoder_GetSamples;
			stage->SetLoop = SplitDecoder_SetLoop;
		}
		else
		{
			if (decoder_selectors[0] != NULL)
				stage->decoder = decoder_selectors[0];
			else
				stage->decoder = decoder_selectors[1];

			stage->Destroy = DecoderSelector_Destroy;
			stage->Rewind = DecoderSelector_Rewind;
			stage->GetSamples = DecoderSelector_GetSamples;
			stage->SetLoop = DecoderSelector_SetLoop;
		}

		// Now for the resampler

		void *resampled_decoder = ResampledDecoder_Create(stage, &wanted_spec, decoder_selectors[0] != NULL ? &specs[0] : &specs[1]);

		if (resampled_decoder == NULL)
		{
			if (split_decoder != NULL)
			{
				SplitDecoder_Destroy(split_decoder);
			}
			else
			{
				if (decoder_selectors[0] != NULL)
					DecoderSelector_Destroy(decoder_selectors[0]);

				if (decoder_selectors[1] != NULL)
					DecoderSelector_Destroy(decoder_selectors[1]);
			}

			free(selector_stages[0]);
			free(selector_stages[1]);
			free(stage);
			return 0;
		}

		stage = (DecoderStage*)malloc(sizeof(DecoderStage));

		if (stage == NULL)
		{
			if (split_decoder != NULL)
			{
				SplitDecoder_Destroy(split_decoder);
			}
			else
			{
				if (decoder_selectors[0] != NULL)
					DecoderSelector_Destroy(decoder_selectors[0]);

				if (decoder_selectors[1] != NULL)
					DecoderSelector_Destroy(decoder_selectors[1]);
			}

			free(selector_stages[0]);
			free(selector_stages[1]);
			free(stage);
			return 0;
		}

		stage->decoder = resampled_decoder;
		stage->Destroy = ResampledDecoder_Destroy;
		stage->Rewind = ResampledDecoder_Rewind;
		stage->GetSamples = ResampledDecoder_GetSamples;
		stage->SetLoop = ResampledDecoder_SetLoop;

		// Finally we're done - now just allocate the channel

		Channel *channel = (Channel*)malloc(sizeof(Channel));

		if (channel == NULL)
		{
			free(stage);

			if (split_decoder != NULL)
			{
				SplitDecoder_Destroy(split_decoder);
			}
			else
			{
				if (decoder_selectors[0] != NULL)
					DecoderSelector_Destroy(decoder_selectors[0]);

				if (decoder_selectors[1] != NULL)
					DecoderSelector_Destroy(decoder_selectors[1]);
			}

			free(selector_stages[0]);
			free(selector_stages[1]);
			free(stage);
			return 0;
		}

		do
		{
			sound = ++mixer->sound_allocator;
		} while (sound == 0);	// Do not let it allocate 0 - it is an error value

		channel->pipeline = stage;
		channel->resampled_decoder = resampled_decoder;
		channel->volume_left = 1.0f;
		channel->volume_right = 1.0f;
		channel->sound = sound;
		channel->paused = true;
		channel->free_when_done = !config->do_not_free_when_done;
		channel->fade_out_counter_max = 0;
		channel->fade_in_counter_max = 0;

		MutexLock(&mixer->mutex);
		channel->next = mixer->channel_list_head;
		mixer->channel_list_head = channel;
		MutexUnlock(&mixer->mutex);
	}

	return sound;
}

CLOWNAUDIO_EXPORT void ClownAudio_DestroySound(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	Channel *channel = NULL;

	MutexLock(&mixer->mutex);

	for (Channel **channel_pointer = &mixer->channel_list_head; *channel_pointer != NULL; channel_pointer = &(*channel_pointer)->next)
	{
		if ((*channel_pointer)->sound == sound)
		{
			channel = *channel_pointer;
			*channel_pointer = channel->next;
			break;
		}
	}

	MutexUnlock(&mixer->mutex);

	if (channel != NULL)
	{
		channel->pipeline->Destroy(channel->pipeline->decoder);
		free(channel);
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_RewindSound(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
		channel->pipeline->Rewind(channel->pipeline->decoder);

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_PauseSound(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
		channel->paused = true;

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnpauseSound(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
		channel->paused = false;

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeOutSound(ClownAudio_Mixer *mixer, ClownAudio_Sound sound, unsigned int duration)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
	{
		unsigned long new_fade_out_counter_max = (mixer->sample_rate * duration) / 1000;

		if (channel->fade_in_counter_max != 0)
			channel->fade_counter = (unsigned long)((channel->fade_in_counter_max - channel->fade_counter) * ((float)new_fade_out_counter_max / (float)channel->fade_in_counter_max));
		else if (channel->fade_out_counter_max != 0)
			channel->fade_counter = (unsigned long)(channel->fade_counter * ((float)new_fade_out_counter_max / (float)channel->fade_out_counter_max));
		else
			channel->fade_counter = new_fade_out_counter_max;

		channel->fade_out_counter_max = new_fade_out_counter_max;
		channel->fade_in_counter_max = 0;
	}

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeInSound(ClownAudio_Mixer *mixer, ClownAudio_Sound sound, unsigned int duration)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
	{
		unsigned long new_fade_in_counter_max = (mixer->sample_rate * duration) / 1000;

		if (channel->fade_out_counter_max != 0)
			channel->fade_counter = (unsigned long)((channel->fade_out_counter_max - channel->fade_counter) * ((float)new_fade_in_counter_max / (float)channel->fade_out_counter_max));
		else if (channel->fade_in_counter_max != 0)
			channel->fade_counter = (unsigned long)(channel->fade_counter * ((float)new_fade_in_counter_max / (float)channel->fade_in_counter_max));
		else
			channel->fade_counter = new_fade_in_counter_max;

		channel->fade_in_counter_max = new_fade_in_counter_max;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_CancelFade(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
	{
		channel->fade_in_counter_max = 0;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT int ClownAudio_GetSoundStatus(ClownAudio_Mixer *mixer, ClownAudio_Sound sound)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	int status = (channel == NULL) ? -1 : channel->paused;

	MutexUnlock(&mixer->mutex);

	return status;
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundVolume(ClownAudio_Mixer *mixer, ClownAudio_Sound sound, float volume_left, float volume_right)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
	{
		channel->volume_left = volume_left;
		channel->volume_right = volume_right;
	}

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundLoop(ClownAudio_Mixer *mixer, ClownAudio_Sound sound, bool loop)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
		channel->pipeline->SetLoop(channel->pipeline->decoder, loop);

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundSampleRate(ClownAudio_Mixer *mixer, ClownAudio_Sound sound, unsigned long sample_rate)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, sound);

	if (channel != NULL)
		ResampledDecoder_SetSampleRate(channel->resampled_decoder, sample_rate);

	MutexUnlock(&mixer->mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_MixSamples(ClownAudio_Mixer *mixer, float *output_buffer, size_t frames_to_do)
{
	MutexLock(&mixer->mutex);

	Channel **channel_pointer = &mixer->channel_list_head;
	while (*channel_pointer != NULL)
	{
		Channel *channel = *channel_pointer;

		if (channel->paused == false)
		{
			float *output_buffer_pointer = output_buffer;

			size_t frames_done = 0;
			for (size_t sub_frames_done; frames_done < frames_to_do; frames_done += sub_frames_done)
			{
				float read_buffer[0x1000];

				const size_t sub_frames_to_do = MIN(0x1000 / CHANNEL_COUNT, frames_to_do - frames_done);
				sub_frames_done = channel->pipeline->GetSamples(channel->pipeline->decoder, read_buffer, sub_frames_to_do);

				float *read_buffer_pointer = read_buffer;

				for (size_t i = 0; i < sub_frames_done; ++i)
				{
					float fade_volume = 1.0f;

					// Apply fade-out volume
					if (channel->fade_out_counter_max != 0)
					{
						const float fade_out_volume = channel->fade_counter / (float)channel->fade_out_counter_max;

						fade_volume *= (fade_out_volume * fade_out_volume);	// Fade logarithmically

						if (channel->fade_counter != 0)
							--channel->fade_counter;
					}

					// Apply fade-in volume
					if (channel->fade_in_counter_max != 0)
					{
						const float fade_in_volume = (channel->fade_in_counter_max - channel->fade_counter) / (float)channel->fade_in_counter_max;

						fade_volume *= (fade_in_volume * fade_in_volume);	// Fade logarithmically

						if (--channel->fade_counter == 0)
							channel->fade_in_counter_max = 0;
					}

					// Mix data with output, and apply volume
					*output_buffer_pointer++ += *read_buffer_pointer++ * channel->volume_left * fade_volume;
					*output_buffer_pointer++ += *read_buffer_pointer++ * channel->volume_right * fade_volume;
				}

				if (sub_frames_done < sub_frames_to_do)
				{
					frames_done += sub_frames_done;
					break;
				}
			}

			if (frames_done < frames_to_do)	// Sound finished
			{
				if (channel->free_when_done)
				{
					channel->pipeline->Destroy(channel->pipeline->decoder);
					*channel_pointer = channel->next;
					free(channel);
					continue;
				}
				else
				{
					channel->paused = true;
				}
			}
		}

		channel_pointer = &(*channel_pointer)->next;
	}

	MutexUnlock(&mixer->mutex);
}
