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

#include "predecoder.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Channel
{
	struct Channel *next;

	bool paused;
	float volume;
	Predecoder *predecoder;
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

static Channel *channel_list_head;

static Mutex mixer_mutex;

static unsigned long output_sample_rate;
static unsigned int output_channel_count;

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

static Channel* FindChannel(Mixer_Sound instance)
{
	for (Channel *channel = channel_list_head; channel != NULL; channel = channel->next)
		if (channel->instance == instance)
			return channel;

	return NULL;
}

void Mixer_Init(unsigned long sample_rate, unsigned int channel_count)
{
	output_sample_rate = sample_rate;
	output_channel_count = channel_count;

	MutexInit(&mixer_mutex);
}

void Mixer_Deinit(void)
{
	MutexDeinit(&mixer_mutex);
}

Mixer_SoundData* Mixer_LoadSoundData(const unsigned char *file_buffer, size_t file_size/*, bool predecode*/)
{
	return (Mixer_SoundData*)Predecoder_DecodeData(file_buffer, file_size, output_sample_rate, output_channel_count);
}

void Mixer_UnloadSoundData(Mixer_SoundData *sound)
{
	Predecoder_UnloadData((PredecoderData*)sound);
}

Mixer_Sound Mixer_CreateSound(Mixer_SoundData *sound, bool loop)
{
	static Mixer_Sound instance_allocator;

	Mixer_Sound instance = 0;	// TODO: This is an error value - never let instance_allocator generate it

	Predecoder *predecoder = Predecoder_Create((PredecoderData*)sound, loop);

	if (predecoder != NULL)
	{
		instance = ++instance_allocator;

		Channel *channel = malloc(sizeof(Channel));

		channel->predecoder = predecoder;
		channel->volume = 1.0f;
		channel->instance = instance;
		channel->paused = true;
		channel->fade_out_counter_max = 0;
		channel->fade_in_counter_max = 0;

		MutexLock(&mixer_mutex);
		channel->next = channel_list_head;
		channel_list_head = channel;
		MutexUnlock(&mixer_mutex);
	}

	return instance;
}

void Mixer_DestroySound(Mixer_Sound instance)
{
	Channel *channel = NULL;

	MutexLock(&mixer_mutex);

	for (Channel **channel_pointer = &channel_list_head; *channel_pointer != NULL; channel_pointer = &(*channel_pointer)->next)
	{
		if ((*channel_pointer)->instance == instance)
		{
			channel = *channel_pointer;
			*channel_pointer = channel->next;
			break;
		}
	}

	MutexUnlock(&mixer_mutex);

	if (channel != NULL)
	{
		Predecoder_Destroy(channel->predecoder);
		free(channel);
	}
}

void Mixer_PauseSound(Mixer_Sound instance)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel != NULL)
		channel->paused = true;

	MutexUnlock(&mixer_mutex);
}

void Mixer_UnpauseSound(Mixer_Sound instance)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel != NULL)
		channel->paused = false;

	MutexUnlock(&mixer_mutex);
}

void Mixer_FadeOutSound(Mixer_Sound instance, unsigned int duration)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel != NULL)
	{
		channel->fade_out_counter_max = (output_sample_rate * duration) / 1000;
		channel->fade_counter = channel->fade_in_counter_max ? (((unsigned long long)channel->fade_in_counter_max - channel->fade_counter) * channel->fade_out_counter_max) / channel->fade_in_counter_max : channel->fade_out_counter_max;
		channel->fade_in_counter_max = 0;
	}

	MutexUnlock(&mixer_mutex);
}

void Mixer_FadeInSound(Mixer_Sound instance, unsigned int duration)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel != NULL)
	{
		channel->fade_in_counter_max = (output_sample_rate * duration) / 1000;
		channel->fade_counter = channel->fade_out_counter_max ? (((unsigned long long)channel->fade_out_counter_max - channel->fade_counter) * channel->fade_in_counter_max) / channel->fade_out_counter_max : channel->fade_in_counter_max;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer_mutex);
}

void Mixer_CancelFade(Mixer_Sound instance)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel != NULL)
	{
		channel->fade_in_counter_max = 0;
		channel->fade_out_counter_max = 0;
	}

	MutexUnlock(&mixer_mutex);
}

void Mixer_SetSoundVolume(Mixer_Sound instance, float volume)
{
	MutexLock(&mixer_mutex);

	Channel *channel = FindChannel(instance);

	if (channel != NULL)
		channel->volume = volume * volume;

	MutexUnlock(&mixer_mutex);
}

void Mixer_MixSamples(float *output_buffer, unsigned long frames_to_do)
{
	MutexLock(&mixer_mutex);

	Channel **channel_pointer = &channel_list_head;
	while (*channel_pointer != NULL)
	{
		Channel *channel = *channel_pointer;

		if (channel->paused == false)
		{
			float *output_buffer_pointer = output_buffer;

			unsigned long frames_done = 0;
			for (unsigned long sub_frames_done; frames_done < frames_to_do; frames_done += sub_frames_done)
			{
				float read_buffer[0x1000];

				const unsigned long sub_frames_to_do = MIN(0x1000 / output_channel_count, frames_to_do - frames_done);
				sub_frames_done = Predecoder_GetSamples(channel->predecoder, read_buffer, sub_frames_to_do);

				float *read_buffer_pointer = read_buffer;

				for (unsigned long i = 0; i < sub_frames_done; ++i)
				{
					float volume = channel->volume;

					if (channel->fade_out_counter_max)
					{
						const float fade_out_volume = channel->fade_counter / (float)channel->fade_out_counter_max;

						volume *= (fade_out_volume * fade_out_volume);

						if (channel->fade_counter)
							--channel->fade_counter;
					}

					if (channel->fade_in_counter_max)
					{
						const float fade_in_volume = (channel->fade_in_counter_max - channel->fade_counter) / (float)channel->fade_in_counter_max;

						volume *= (fade_in_volume * fade_in_volume);

						if (--channel->fade_counter == 0)
							channel->fade_in_counter_max = 0;
					}

					for (unsigned int j = 0; j < output_channel_count; ++j)
						*output_buffer_pointer++ += *read_buffer_pointer++ * volume;
				}

				if (sub_frames_done < sub_frames_to_do)
				{
					frames_done += sub_frames_done;
					break;
				}
			}

			if (frames_done < frames_to_do)	// Sound finished
			{
				Predecoder_Destroy(channel->predecoder);
				*channel_pointer = channel->next;
				free(channel);
				continue;
			}
		}

		channel_pointer = &(*channel_pointer)->next;
	}

	MutexUnlock(&mixer_mutex);
}
