#include "playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

struct BackendStream
{
	void (*user_callback)(void*, float*, unsigned long);
	void *user_data;

	float volume;
};

static bool sdl_already_init;

static unsigned int NextPowerOfTwo(unsigned int value)
{
	value--;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	value++;

	return value;
}

static void Callback(void *user_data, Uint8 *output_buffer_uint8, int bytes_to_do)
{
	BackendStream *stream = user_data;
	const unsigned long frames_to_do = bytes_to_do / (sizeof(short) * STREAM_CHANNEL_COUNT);

	// This playback backend doesn't support float32, so we have to convert the samples to S16
	short *output_buffer_pointer = (short*)output_buffer_uint8;
	for (unsigned long frames_done = 0; frames_done < frames_to_do; frames_done += 0x1000)
	{
		float read_buffer[0x1000 * STREAM_CHANNEL_COUNT];

		stream->user_callback(stream->user_data, read_buffer, MIN(0x1000, frames_to_do - frames_done));

		float *read_buffer_pointer = read_buffer;
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
		{
			float sample = *read_buffer_pointer++;

			if (sample > 1.0f)
				sample = 1.0f;
			else if (sample < -1.0f)
				sample = -1.0f;

			*output_buffer_pointer++ = (short)(sample * stream->volume * 32767.0f);
		}
	}
}

bool Backend_Init(void)
{
	bool success = true;

	sdl_already_init = SDL_WasInit(SDL_INIT_AUDIO);

	if (!sdl_already_init)
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) == -1)
			success = false;

	return success;
}

void Backend_Deinit(void)
{
	if (!sdl_already_init)
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, unsigned long), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	if (stream != NULL)
	{
		SDL_AudioSpec want;
		memset(&want, 0, sizeof(want));
		want.freq = STREAM_SAMPLE_RATE;
		want.format = AUDIO_S16;
		want.channels = STREAM_CHANNEL_COUNT;
		want.samples = NextPowerOfTwo(((STREAM_SAMPLE_RATE * 10) / 1000) * STREAM_CHANNEL_COUNT);	// A low-latency buffer of 10 milliseconds
		want.callback = Callback;
		want.userdata = stream;

		if (SDL_OpenAudio(&want, NULL) == 0)
		{
			stream->user_callback = user_callback;
			stream->user_data = user_data;

			stream->volume = 1.0f;
		}
		else
		{
			free(stream);
			stream = NULL;
		}
	}

	return stream;
}

bool Backend_DestroyStream(BackendStream *stream)
{
	if (stream != NULL)
	{
		SDL_CloseAudio();
		free(stream);
	}

	return true;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	if (stream != NULL)
		stream->volume = volume * volume;

	return true;
}

bool Backend_PauseStream(BackendStream *stream)
{
	if (stream != NULL)
		SDL_PauseAudio(-1);

	return true;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	if (stream != NULL)
		SDL_PauseAudio(0);

	return true;
}
