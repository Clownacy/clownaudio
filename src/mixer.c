// (C) 2019-2021 Clownacy
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#include "clownaudio/mixer.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "decoding/decoders/common.h"

#include "decoding/decoder_selector.h"
#include "decoding/resampled_decoder.h"
#include "decoding/split_decoder.h"

#define CHANNEL_COUNT 2

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, min, max) MIN(MAX((x), (min)), (max))

#define SCALE(x, scale) (((x) * (scale)) >> 8)

#define COUNT_OF(array) (sizeof(array) / sizeof(*array))

struct ClownAudio_Mixer
{
	ClownAudio_Sound *sound_hash_table[0x100];
	ClownAudio_Sound *playing_list_head;
	unsigned long sample_rate;
	ClownAudio_SoundID sound_id_allocator;
};

struct ClownAudio_Sound
{
	// List of sounds in bucket
	struct ClownAudio_Sound *prev_in_bucket;
	struct ClownAudio_Sound *next_in_bucket;

	// List of all currently-playing sounds
	struct ClownAudio_Sound *prev_playing;
	struct ClownAudio_Sound *next_playing;

	// List of sounds created from a single sound data
	struct ClownAudio_Sound *prev_sibling;
	struct ClownAudio_Sound *next_sibling;

	bool paused;
	bool destroy_when_done;
	unsigned short volume_left;
	unsigned short volume_right;
	DecoderStage pipeline;
	void *resampled_decoders[2];
	ClownAudio_SoundID id;

	unsigned long fade_out_counter_max;
	unsigned long fade_in_counter_max;
	unsigned long fade_counter;
};

struct ClownAudio_SoundData
{
	ClownAudio_Sound sound_list_sentinel;

	DecoderSelectorData *decoder_selector_data[2];
	unsigned char *file_buffers[2];
};

static bool LoadFileToMemory(const char *path, unsigned char **buffer, size_t *size)
{
	bool success = false;

	if (path == NULL || path[0] == '\0')
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

static void AddSoundToPlayingList(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound)
{
	sound->prev_playing = NULL;
	sound->next_playing = mixer->playing_list_head;

	if (mixer->playing_list_head != NULL)
		mixer->playing_list_head->prev_playing = sound;

	mixer->playing_list_head = sound;
}

static void RemoveSoundFromPlayingList(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound)
{
	if (sound->prev_playing != NULL)
		sound->prev_playing->next_playing = sound->next_playing;
	else
		mixer->playing_list_head = sound->next_playing;

	if (sound->next_playing != NULL)
		sound->next_playing->prev_playing = sound->prev_playing;
}

static ClownAudio_Sound* FindSound(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	for (ClownAudio_Sound *sound = mixer->sound_hash_table[sound_id % COUNT_OF(mixer->sound_hash_table)]; sound != NULL; sound = sound->next_in_bucket)
		if (sound->id == sound_id)
			return sound;

	return NULL;
}

static void DestroySound(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound)
{
	if (!sound->paused)
		RemoveSoundFromPlayingList(mixer, sound);

	// Detach sound from sound list
	if (sound->prev_in_bucket != NULL)
		sound->prev_in_bucket->next_in_bucket = sound->next_in_bucket;
	else
		mixer->sound_hash_table[sound->id % COUNT_OF(mixer->sound_hash_table)] = sound->next_in_bucket;

	if (sound->next_in_bucket != NULL)
		sound->next_in_bucket->prev_in_bucket = sound->prev_in_bucket;

	// Detach sound from list of sounds derived from the same sound data
	sound->prev_sibling->next_sibling = sound->next_sibling;

	if (sound->next_sibling != NULL)
		sound->next_sibling->prev_sibling = sound->prev_sibling;

	sound->pipeline.Destroy(sound->pipeline.decoder);
	free(sound);
}

static void PauseSound(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound)
{
	if (!sound->paused)
	{
		RemoveSoundFromPlayingList(mixer, sound);
		sound->paused = true;
	}
}

static void UnpauseSound(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound)
{
	if (sound->paused)
	{
		AddSoundToPlayingList(mixer, sound);
		sound->paused = false;
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundDataConfigInit(ClownAudio_SoundDataConfig *config)
{
	config->predecode = false;
	config->must_predecode = false;
	config->dynamic_sample_rate = false;
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundConfigInit(ClownAudio_SoundConfig *config)
{
	config->loop = false;
	config->do_not_destroy_when_done = false;
	config->dynamic_sample_rate = false;
}

CLOWNAUDIO_EXPORT ClownAudio_Mixer* ClownAudio_Mixer_Create(unsigned long sample_rate)
{
	ClownAudio_Mixer *mixer = (ClownAudio_Mixer*)malloc(sizeof(ClownAudio_Mixer));

	if (mixer != NULL)
	{
		for (size_t i = 0; i < COUNT_OF(mixer->sound_hash_table); ++i)
			mixer->sound_hash_table[i] = NULL;

		mixer->playing_list_head = NULL;

		mixer->sample_rate = sample_rate;

		mixer->sound_id_allocator = 0;
	}

	return mixer;
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_Destroy(ClownAudio_Mixer *mixer)
{
	free(mixer);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_Mixer_SoundDataLoadFromMemory(ClownAudio_Mixer *mixer, const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	ClownAudio_SoundData *sound_data = (ClownAudio_SoundData*)malloc(sizeof(ClownAudio_SoundData));

	if (sound_data != NULL)
	{
		DecoderSpec wanted_spec;

		wanted_spec.sample_rate = config->dynamic_sample_rate ? 0 : mixer->sample_rate;	// Do not change the sample rate when dynamic resampling is enabled
		wanted_spec.channel_count = CHANNEL_COUNT;

		sound_data->sound_list_sentinel.next_sibling = NULL;

		sound_data->file_buffers[0] = NULL;
		sound_data->file_buffers[1] = NULL;

		if (file_buffer1 != NULL && file_buffer2 != NULL)
		{
			sound_data->decoder_selector_data[0] = DecoderSelector_LoadData(file_buffer1, file_size1, config->predecode, config->must_predecode, &wanted_spec);
			sound_data->decoder_selector_data[1] = DecoderSelector_LoadData(file_buffer2, file_size2, config->predecode, config->must_predecode, &wanted_spec);

			if (sound_data->decoder_selector_data[0] != NULL && sound_data->decoder_selector_data[1] != NULL)
				return sound_data;

			if (sound_data->decoder_selector_data[0] != NULL)
				DecoderSelector_UnloadData(sound_data->decoder_selector_data[0]);

			if (sound_data->decoder_selector_data[1] != NULL)
				DecoderSelector_UnloadData(sound_data->decoder_selector_data[1]);
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

		free(sound_data);
	}

	return NULL;
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_Mixer_SoundDataLoadFromFiles(ClownAudio_Mixer *mixer, const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config)
{
	if ((intro_path != NULL && intro_path[0] != '\0') || (loop_path != NULL && loop_path[0] != '\0'))
	{
		unsigned char *file_buffers[2];
		size_t file_buffer_sizes[2];

		if (LoadFileToMemory(intro_path, &file_buffers[0], &file_buffer_sizes[0]))
		{
			if (LoadFileToMemory(loop_path, &file_buffers[1], &file_buffer_sizes[1]))
			{
				ClownAudio_SoundData *sound_data = ClownAudio_Mixer_SoundDataLoadFromMemory(mixer, file_buffers[0], file_buffer_sizes[0], file_buffers[1], file_buffer_sizes[1], config);

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

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundDataUnload(ClownAudio_Mixer *mixer, ClownAudio_SoundData *sound_data)
{
	if (sound_data != NULL)
	{
		// Destroy any sounds that use this sound data
		for (ClownAudio_Sound *sound = sound_data->sound_list_sentinel.next_sibling; sound != NULL; )
		{
			ClownAudio_Sound *next_sound = sound->next_playing; // A work-around to avoid using `sound` after it is freed

			DestroySound(mixer, sound);

			sound = next_sound;
		}

		if (sound_data->decoder_selector_data[0] != NULL)
			DecoderSelector_UnloadData(sound_data->decoder_selector_data[0]);

		if (sound_data->decoder_selector_data[1] != NULL)
			DecoderSelector_UnloadData(sound_data->decoder_selector_data[1]);

		free(sound_data->file_buffers[0]);
		free(sound_data->file_buffers[1]);

		free(sound_data);
	}
}

CLOWNAUDIO_EXPORT ClownAudio_Sound* ClownAudio_Mixer_SoundCreate(ClownAudio_Mixer *mixer, ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config)
{
	if (sound_data != NULL)
	{
		DecoderSpec wanted_spec;
		wanted_spec.sample_rate = config->dynamic_sample_rate ? 0 : mixer->sample_rate;	// If 'dynamic_sample_rate' is enabled, make the decoder backend use its own fixed sample rate
		wanted_spec.channel_count = CHANNEL_COUNT;

		// Begin constructing the decoder pipeline

		DecoderStage stage;

		// Let's start with the decoder-selectors

		DecoderStage selector_stages[2];

		void *decoder_selectors[2] = {NULL, NULL};
		DecoderSpec specs[2];

		if (sound_data->decoder_selector_data[0] != NULL)
		{
			decoder_selectors[0] = DecoderSelector_Create(sound_data->decoder_selector_data[0], sound_data->decoder_selector_data[1] != NULL ? false : config->loop, &wanted_spec, &specs[0]);

			if (decoder_selectors[0] != NULL)
			{
				selector_stages[0].decoder = decoder_selectors[0];
				selector_stages[0].Destroy = DecoderSelector_Destroy;
				selector_stages[0].Rewind = DecoderSelector_Rewind;
				selector_stages[0].GetSamples = DecoderSelector_GetSamples;
				selector_stages[0].SetLoop = DecoderSelector_SetLoop;
			}
		}

		if (sound_data->decoder_selector_data[1] != NULL)
		{
			decoder_selectors[1] = DecoderSelector_Create(sound_data->decoder_selector_data[1], config->loop, &wanted_spec, &specs[1]);

			if (decoder_selectors[1] != NULL)
			{
				selector_stages[1].decoder = decoder_selectors[1];
				selector_stages[1].Destroy = DecoderSelector_Destroy;
				selector_stages[1].Rewind = DecoderSelector_Rewind;
				selector_stages[1].GetSamples = DecoderSelector_GetSamples;
				selector_stages[1].SetLoop = DecoderSelector_SetLoop;
			}
		}

		if (decoder_selectors[0] == NULL && decoder_selectors[1] == NULL)
			return NULL;

		// Now for the resampler(s)

		wanted_spec.sample_rate = mixer->sample_rate;	// Now update the sample rate, so the resampler converts to the mixer's expected rate

		DecoderStage resampled_stages[2];

		void *resampled_decoders[2] = {NULL, NULL};

		for (unsigned int i = 0; i < 2; ++i)
		{
			if (decoder_selectors[i] != NULL)
			{
				resampled_decoders[i] = ResampledDecoder_Create(&selector_stages[i], config->dynamic_sample_rate, &wanted_spec, &specs[i]);

				if (resampled_decoders[i] == NULL)
				{
					if (decoder_selectors[0] != NULL)
						DecoderSelector_Destroy(decoder_selectors[0]);

					if (decoder_selectors[1] != NULL)
						DecoderSelector_Destroy(decoder_selectors[1]);

					return NULL;
				}

				resampled_stages[i].decoder = resampled_decoders[i];
				resampled_stages[i].Destroy = ResampledDecoder_Destroy;
				resampled_stages[i].Rewind = ResampledDecoder_Rewind;
				resampled_stages[i].GetSamples = ResampledDecoder_GetSamples;
				resampled_stages[i].SetLoop = ResampledDecoder_SetLoop;
			}
		}

		// Now for the split-decoder, if needed

		void *split_decoder = NULL;

		if (decoder_selectors[0] != NULL && decoder_selectors[1] != NULL)
		{
			split_decoder = SplitDecoder_Create(&resampled_stages[0], &resampled_stages[1], CHANNEL_COUNT);

			if (split_decoder == NULL)
			{
				ResampledDecoder_Destroy(resampled_decoders[0]);
				ResampledDecoder_Destroy(resampled_decoders[1]);
				return NULL;
			}

			stage.decoder = split_decoder;
			stage.Destroy = SplitDecoder_Destroy;
			stage.Rewind = SplitDecoder_Rewind;
			stage.GetSamples = SplitDecoder_GetSamples;
			stage.SetLoop = SplitDecoder_SetLoop;
		}
		else
		{
			if (resampled_decoders[0] != NULL)
				stage.decoder = resampled_decoders[0];
			else
				stage.decoder = resampled_decoders[1];

			stage.Destroy = ResampledDecoder_Destroy;
			stage.Rewind = ResampledDecoder_Rewind;
			stage.GetSamples = ResampledDecoder_GetSamples;
			stage.SetLoop = ResampledDecoder_SetLoop;
		}

		// Finally we're done - now just allocate the sound

		ClownAudio_Sound *sound = (ClownAudio_Sound*)malloc(sizeof(ClownAudio_Sound));

		if (sound == NULL)
		{
			stage.Destroy(stage.decoder);
			return NULL;
		}

		sound->pipeline = stage;
		sound->resampled_decoders[0] = resampled_decoders[0];
		sound->resampled_decoders[1] = resampled_decoders[1];
		sound->volume_left = 0x100;
		sound->volume_right = 0x100;
		sound->paused = true;
		sound->destroy_when_done = !config->do_not_destroy_when_done;
		sound->fade_out_counter_max = 0;
		sound->fade_in_counter_max = 0;

		return sound;
	}

	return NULL;
}

CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_Mixer_SoundRegister(ClownAudio_Mixer *mixer, ClownAudio_Sound *sound, ClownAudio_SoundData *sound_data)
{
	ClownAudio_SoundID sound_id = 0;

	if (sound != NULL)
	{
		do
		{
			sound_id = ++mixer->sound_id_allocator;
		} while (sound_id == 0);	// Do not let it allocate 0 - it is an error value

		sound->id = sound_id;

		// Add sound to hash table of all sound
		ClownAudio_Sound **sound_list_head_pointer = &mixer->sound_hash_table[sound_id % COUNT_OF(mixer->sound_hash_table)];
		ClownAudio_Sound *sound_list_head = *sound_list_head_pointer;

		sound->prev_in_bucket = NULL;
		sound->next_in_bucket = sound_list_head;

		if (sound_list_head != NULL)
			sound_list_head->prev_in_bucket = sound;

		*sound_list_head_pointer = sound;

		// Add sound to list of sounds derived from the same sound data
		sound->prev_sibling = &sound_data->sound_list_sentinel;
		sound->next_sibling = sound_data->sound_list_sentinel.next_sibling;

		if (sound_data->sound_list_sentinel.next_sibling != NULL)
			sound_data->sound_list_sentinel.next_sibling->prev_sibling = sound;

		sound_data->sound_list_sentinel.next_sibling = sound;
	}

	return sound_id;
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundDestroy(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
		DestroySound(mixer, sound);
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundRewind(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
		sound->pipeline.Rewind(sound->pipeline.decoder);
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundPause(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
		PauseSound(mixer, sound);
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundUnpause(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
		UnpauseSound(mixer, sound);
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundFadeOut(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
	{
		unsigned long new_fade_out_counter_max = (mixer->sample_rate * duration) / 1000;

		if (sound->fade_in_counter_max != 0)
			sound->fade_counter = (unsigned long)((sound->fade_in_counter_max - sound->fade_counter) * ((float)new_fade_out_counter_max / (float)sound->fade_in_counter_max));
		else if (sound->fade_out_counter_max != 0)
			sound->fade_counter = (unsigned long)(sound->fade_counter * ((float)new_fade_out_counter_max / (float)sound->fade_out_counter_max));
		else
			sound->fade_counter = new_fade_out_counter_max;

		sound->fade_out_counter_max = new_fade_out_counter_max;
		sound->fade_in_counter_max = 0;
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundFadeIn(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
	{
		unsigned long new_fade_in_counter_max = (mixer->sample_rate * duration) / 1000;

		if (sound->fade_out_counter_max != 0)
			sound->fade_counter = (unsigned long)((sound->fade_out_counter_max - sound->fade_counter) * ((float)new_fade_in_counter_max / (float)sound->fade_out_counter_max));
		else if (sound->fade_in_counter_max != 0)
			sound->fade_counter = (unsigned long)(sound->fade_counter * ((float)new_fade_in_counter_max / (float)sound->fade_in_counter_max));
		else
			sound->fade_counter = new_fade_in_counter_max;

		sound->fade_in_counter_max = new_fade_in_counter_max;
		sound->fade_out_counter_max = 0;
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundCancelFade(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
	{
		sound->fade_in_counter_max = 0;
		sound->fade_out_counter_max = 0;
	}
}

CLOWNAUDIO_EXPORT int ClownAudio_Mixer_SoundGetStatus(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	return (sound == NULL) ? -1 : sound->paused;
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundSetVolume(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
	{
		sound->volume_left = volume_left;
		sound->volume_right = volume_right;
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundSetLoop(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, bool loop)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
		sound->pipeline.SetLoop(sound->pipeline.decoder, loop);
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_SoundSetSpeed(ClownAudio_Mixer *mixer, ClownAudio_SoundID sound_id, unsigned long speed)
{
	ClownAudio_Sound *sound = FindSound(mixer, sound_id);

	if (sound != NULL)
	{
		if (sound->resampled_decoders[0] != NULL)
			ResampledDecoder_SetSpeed(sound->resampled_decoders[0], speed);

		if (sound->resampled_decoders[1] != NULL)
			ResampledDecoder_SetSpeed(sound->resampled_decoders[1], speed);
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_MixSamples(ClownAudio_Mixer *mixer, long *output_buffer, size_t frames_to_do)
{
	ClownAudio_Sound *sound = mixer->playing_list_head;

	while (sound != NULL)
	{
		ClownAudio_Sound *next_sound = sound->next_playing;

		long *output_buffer_pointer = output_buffer;

		size_t frames_done = 0;
		for (size_t sub_frames_done; frames_done < frames_to_do; frames_done += sub_frames_done)
		{
			short read_buffer[0x1000];

			const size_t sub_frames_to_do = MIN(0x1000 / CHANNEL_COUNT, frames_to_do - frames_done);
			sub_frames_done = sound->pipeline.GetSamples(sound->pipeline.decoder, read_buffer, sub_frames_to_do);

			short *read_buffer_pointer = read_buffer;

			for (size_t i = 0; i < sub_frames_done; ++i)
			{
				unsigned short fade_volume = 0x100;

				// Apply fade-out volume
				if (sound->fade_out_counter_max != 0)
				{
					const unsigned short fade_out_volume = (sound->fade_counter << 8) / sound->fade_out_counter_max;

					fade_volume = SCALE(fade_volume, SCALE(fade_out_volume, fade_out_volume));	// Fade logarithmically

					if (sound->fade_counter != 0)
						--sound->fade_counter;
				}

				// Apply fade-in volume
				if (sound->fade_in_counter_max != 0)
				{
					const unsigned short fade_in_volume = ((sound->fade_in_counter_max - sound->fade_counter) << 8) / (float)sound->fade_in_counter_max;

					fade_volume = SCALE(fade_volume, SCALE(fade_in_volume, fade_in_volume));	// Fade logarithmically

					if (--sound->fade_counter == 0)
						sound->fade_in_counter_max = 0;
				}

				// Mix data with output, and apply volume
				*output_buffer_pointer++ += SCALE(SCALE(*read_buffer_pointer++, sound->volume_left), fade_volume);
				*output_buffer_pointer++ += SCALE(SCALE(*read_buffer_pointer++, sound->volume_right), fade_volume);
			}

			if (sub_frames_done < sub_frames_to_do)
			{
				frames_done += sub_frames_done;
				break;
			}
		}

		if (frames_done < frames_to_do)	// Sound finished
		{
			if (sound->destroy_when_done)
				DestroySound(mixer, sound);
			else
				PauseSound(mixer, sound);
		}

		sound = next_sound;
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_Mixer_OutputSamples(ClownAudio_Mixer *mixer, short *output_buffer, size_t frames_to_do)
{
	size_t frames_done = 0;
	while (frames_done < frames_to_do)
	{
		long mix_buffer[0x1000];

		const size_t sub_frames_to_do = MIN(0x1000 / CHANNEL_COUNT, frames_to_do - frames_done);

		memset(mix_buffer, 0, sub_frames_to_do * sizeof(long) * CHANNEL_COUNT);
		ClownAudio_Mixer_MixSamples(mixer, mix_buffer, sub_frames_to_do);

		// Clamp samples to 16-bit range
		for (size_t i = 0; i < sub_frames_to_do * CHANNEL_COUNT; ++i)
		{
			if (mix_buffer[i] > 0x7FFF)
				*output_buffer++ = 0x7FFF;
			else if (mix_buffer[i] < -0x7FFF)
				*output_buffer++ = -0x7FFF;
			else
				*output_buffer++ = (short)mix_buffer[i];
		}

		frames_done += sub_frames_to_do;
	}
}
