#include "../playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#ifdef _WIN32
#include <windows.h>
#endif

#include <cubeb/cubeb.h>

struct BackendStream
{
	void (*user_callback)(void*, float*, unsigned long);
	void *user_data;

	cubeb_stream *cubeb_stream_pointer;
};

static cubeb *cubeb_context;

static long DataCallback(cubeb_stream *c_stream, void *user_data, void const *input_buffer, void *output_buffer, long frames_to_do)
{
	(void)c_stream;
	(void)input_buffer;

	BackendStream *stream = (BackendStream*)user_data;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	return frames_to_do;
}

static void StateCallback(cubeb_stream *stream, void *user_data, cubeb_state state)
{
	(void)stream;
	(void)user_data;
	(void)state;
}

bool Backend_Init(void)
{
#ifdef _WIN32
	CoInitializeEx(NULL, COINIT_MULTITHREADED);	// Cubeb needs us to init COM
#endif

	return cubeb_init(&cubeb_context, NULL, NULL) == CUBEB_OK;
}

void Backend_Deinit(void)
{
	cubeb_destroy(cubeb_context);

#ifdef _WIN32
	CoUninitialize();
#endif
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, unsigned long), void *user_data)
{
	BackendStream *stream = NULL;

	cubeb_stream_params output_params;
	output_params.format = CUBEB_SAMPLE_FLOAT32LE;
	output_params.rate = STREAM_SAMPLE_RATE;
	output_params.prefs = CUBEB_STREAM_PREF_NONE;
	output_params.channels = STREAM_CHANNEL_COUNT;
	output_params.layout = STREAM_CHANNEL_COUNT == 2 ? CUBEB_LAYOUT_STEREO : CUBEB_LAYOUT_MONO;

	uint32_t latency_frames;

	if (cubeb_get_min_latency(cubeb_context, &output_params, &latency_frames) == CUBEB_OK)
	{
		stream = malloc(sizeof(BackendStream));
		cubeb_stream *cubeb_stream_pointer;

		if (cubeb_stream_init(cubeb_context, &cubeb_stream_pointer, NULL, NULL, NULL, NULL, &output_params, latency_frames, DataCallback, StateCallback, stream) == CUBEB_OK)
		{
			stream->user_callback = user_callback;
			stream->user_data = user_data;

			stream->cubeb_stream_pointer = cubeb_stream_pointer;
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
	bool success = true;

	if (stream)
	{
		if (cubeb_stream_stop(stream->cubeb_stream_pointer) == CUBEB_OK)
		{
			cubeb_stream_destroy(stream->cubeb_stream_pointer);
			free(stream);
		}
		else
		{
			success = false;
		}
	}

	return success;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	bool success = true;

	if (stream)
		success = cubeb_stream_set_volume(stream->cubeb_stream_pointer, volume * volume) == CUBEB_OK;

	return success;
}

bool Backend_PauseStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
		success = cubeb_stream_stop(stream->cubeb_stream_pointer) == CUBEB_OK;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream)
		success = cubeb_stream_start(stream->cubeb_stream_pointer) == CUBEB_OK;

	return success;
}
