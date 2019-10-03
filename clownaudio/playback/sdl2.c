#include "../playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"

struct BackendStream
{
	void (*user_callback)(void*, float*, unsigned long);
	void *user_data;

	SDL_AudioDeviceID device;
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
	const unsigned long frames_to_do = bytes_to_do / (sizeof(float) * STREAM_CHANNEL_COUNT);
	float *output_buffer = (float*)output_buffer_uint8;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since SDL2's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;
}

bool Backend_Init(void)
{
	sdl_already_init = SDL_WasInit(SDL_INIT_AUDIO);

	if (!sdl_already_init)
		SDL_InitSubSystem(SDL_INIT_AUDIO);

	return true;
}

void Backend_Deinit(void)
{
	if (!sdl_already_init)
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, unsigned long), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	SDL_AudioSpec want;
	memset(&want, 0, sizeof(want));
	want.freq = STREAM_SAMPLE_RATE;
	want.format = AUDIO_F32;
	want.channels = STREAM_CHANNEL_COUNT;
	want.samples = NextPowerOfTwo(((STREAM_SAMPLE_RATE * 10) / 1000) * STREAM_CHANNEL_COUNT);	// A low-latency buffer of 10 milliseconds
	want.callback = Callback;
	want.userdata = stream;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &want, NULL, 0);

	if (device)
	{
		stream->user_callback = user_callback;
		stream->user_data = user_data;

		stream->device = device;
		stream->volume = 1.0f;
	}
	else
	{
		free(stream);
		stream = NULL;
	}

	return stream;
}

bool Backend_DestroyStream(BackendStream *stream)
{
	if (stream)
	{
		SDL_CloseAudioDevice(stream->device);
		free(stream);
	}

	return true;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	if (stream)
		stream->volume = volume * volume;

	return true;
}

bool Backend_PauseStream(BackendStream *stream)
{
	if (stream)
		SDL_PauseAudioDevice(stream->device, -1);

	return true;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	if (stream)
		SDL_PauseAudioDevice(stream->device, 0);

	return true;
}
