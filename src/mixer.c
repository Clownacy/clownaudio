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

#include "mixer.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

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
	float volume;
	float left_pan[CHANNEL_COUNT];
	float right_pan[CHANNEL_COUNT];
	SplitDecoder *split_decoder;
	Mixer_Sound instance;

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

struct Mixer
{
	Channel *channel_list_head;
	Mutex mutex;
	unsigned long sample_rate;
	Mixer_Sound instance_allocator;
};

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

static Channel* FindChannel(Mixer *mixer, Mixer_Sound instance)
{
	for (Channel *channel = mixer->channel_list_head; channel != NULL; channel = channel->next)
		if (channel->instance == instance)
			return channel;

	return NULL;
}

DLL_API Mixer* Mixer_Create(unsigned long sample_rate)
{
	Mixer *mixer = malloc(sizeof(Mixer));

	if (mixer != NULL)
	{
		mixer->channel_list_head = NULL;

		mixer->sample_rate = sample_rate;

		MutexInit(&mixer->mutex);
	}

	return mixer;
}

DLL_API void Mixer_Destroy(Mixer *mixer)
{
	MutexDeinit(&mixer->mutex);

	free(mixer);
}

DLL_API Mixer_SoundData* Mixer_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode)
{
	return (Mixer_SoundData*)SplitDecoder_LoadData(file_buffer1, file_size1, file_buffer2, file_size2, predecode);
}

DLL_API void Mixer_UnloadSoundData(Mixer_SoundData *sound)
{
	SplitDecoder_UnloadData((SplitDecoderData*)sound);
}

DLL_API Mixer_Sound Mixer_CreateSound(Mixer *mixer, Mixer_SoundData *sound, bool loop, bool free_when_done)
{
	Mixer_Sound instance = 0;	// TODO: This is an error value - never let instance_allocator generate it

	SplitDecoder *split_decoder = SplitDecoder_Create((SplitDecoderData*)sound, loop, mixer->sample_rate);

	if (split_decoder != NULL)
	{
		instance = ++mixer->instance_allocator;

		Channel *channel = malloc(sizeof(Channel));

		channel->split_decoder = split_decoder;
		channel->volume = 1.0f;
		channel->left_pan[0] = 1.0f;
		channel->right_pan[0] = 0.0f;
		channel->left_pan[1] = 0.0f;
		channel->right_pan[1] = 1.0f;
		channel->instance = instance;
		channel->paused = true;
		channel->free_when_done = free_when_done;
		channel->fade_out_counter_max = 0;
		channel->fade_in_counter_max = 0;

		MutexLock(&mixer->mutex);
		channel->next = mixer->channel_list_head;
		mixer->channel_list_head = channel;
		MutexUnlock(&mixer->mutex);
	}

	return instance;
}

DLL_API void Mixer_DestroySound(Mixer *mixer, Mixer_Sound instance)
{
	Channel *channel = NULL;

	MutexLock(&mixer->mutex);

	for (Channel **channel_pointer = &mixer->channel_list_head; *channel_pointer != NULL; channel_pointer = &(*channel_pointer)->next)
	{
		if ((*channel_pointer)->instance == instance)
		{
			channel = *channel_pointer;
			*channel_pointer = channel->next;
			break;
		}
	}

	MutexUnlock(&mixer->mutex);

	if (channel != NULL)
	{
		SplitDecoder_Destroy(channel->split_decoder);
		free(channel);
	}
}

DLL_API void Mixer_RewindSound(Mixer *mixer, Mixer_Sound instance)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
		SplitDecoder_Rewind(channel->split_decoder);

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_PauseSound(Mixer *mixer, Mixer_Sound instance)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
		channel->paused = true;

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_UnpauseSound(Mixer *mixer, Mixer_Sound instance)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
		channel->paused = false;

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_FadeOutSound(Mixer *mixer, Mixer_Sound instance, unsigned int duration)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

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

DLL_API void Mixer_FadeInSound(Mixer *mixer, Mixer_Sound instance, unsigned int duration)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

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

DLL_API void Mixer_CancelFade(Mixer *mixer, Mixer_Sound instance)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
	{
		channel->fade_in_counter_max = 0;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer->mutex);
}

DLL_API int Mixer_GetSoundStatus(Mixer *mixer, Mixer_Sound instance)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	int status = (channel == NULL) ? -1 : channel->paused;

	MutexUnlock(&mixer->mutex);

	return status;
}

DLL_API void Mixer_SetSoundVolume(Mixer *mixer, Mixer_Sound instance, float volume)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
		channel->volume = volume;

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_SetSoundLoop(Mixer *mixer, Mixer_Sound instance, bool loop)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
		SplitDecoder_SetLoop(channel->split_decoder, loop);

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_SetSoundSampleRate(Mixer *mixer, Mixer_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
		SplitDecoder_SetSampleRate(channel->split_decoder, sample_rate1, sample_rate2);

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_SetPan(Mixer *mixer, Mixer_Sound instance, float pan)
{
	MutexLock(&mixer->mutex);

	Channel *channel = FindChannel(mixer, instance);

	if (channel != NULL)
	{
		channel->left_pan[0] = 1.0f - CLAMP(pan, 0.0f, 1.0f);
		channel->right_pan[1] = 1.0f - CLAMP(-pan, 0.0f, 1.0f);

		// Logarithmic (realistic) panning
		channel->left_pan[0] *= channel->left_pan[0];
		channel->right_pan[1] *= channel->right_pan[1];

		channel->right_pan[0] = 1.0f - channel->right_pan[1];
		channel->left_pan[1] = 1.0f - channel->left_pan[0];
	}

	MutexUnlock(&mixer->mutex);
}

DLL_API void Mixer_MixSamples(Mixer *mixer, float *output_buffer, size_t frames_to_do)
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
				sub_frames_done = SplitDecoder_GetSamples(channel->split_decoder, read_buffer, sub_frames_to_do);

				float *read_buffer_pointer = read_buffer;

				for (size_t i = 0; i < sub_frames_done; ++i)
				{
					float volume = channel->volume;

					// Apply fade-out volume
					if (channel->fade_out_counter_max != 0)
					{
						const float fade_out_volume = channel->fade_counter / (float)channel->fade_out_counter_max;

						volume *= (fade_out_volume * fade_out_volume);

						if (channel->fade_counter != 0)
							--channel->fade_counter;
					}

					// Apply fade-in volume
					if (channel->fade_in_counter_max != 0)
					{
						const float fade_in_volume = (channel->fade_in_counter_max - channel->fade_counter) / (float)channel->fade_in_counter_max;

						volume *= (fade_in_volume * fade_in_volume);

						if (--channel->fade_counter == 0)
							channel->fade_in_counter_max = 0;
					}

					// Mix data with output, and apply volume
					const float left_sample = *read_buffer_pointer++;
					const float right_sample = *read_buffer_pointer++;

					for (unsigned int j = 0; j < CHANNEL_COUNT; ++j)
					{
						float panned_sample = (left_sample * channel->left_pan[j]) + (right_sample * channel->right_pan[j]);

						const float total_pan = channel->left_pan[j] + channel->right_pan[j];
						if (total_pan > 1.0f)
							panned_sample /= total_pan;	// Clamp back down to the original max volume

						*output_buffer_pointer++ += panned_sample * volume;
					}
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
					SplitDecoder_Destroy(channel->split_decoder);
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
