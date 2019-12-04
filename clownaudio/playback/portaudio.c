#include "playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "portaudio.h"

struct BackendStream
{
	void (*user_callback)(void*, float*, size_t);
	void *user_data;

	PaStream *pa_stream;
	float volume;
};

static int Callback(const void *input_buffer, void *output_buffer_void, unsigned long frames_to_do, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
	(void)input_buffer;
	(void)time_info;
	(void)status_flags;

	BackendStream *stream = user_data;
	float *output_buffer = output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since PortAudio's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;

	return 0;
}

bool Backend_Init(void)
{
	return Pa_Initialize() == paNoError;
}

void Backend_Deinit(void)
{
	Pa_Terminate();
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, size_t), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	if (stream != NULL)
	{
		if (Pa_OpenDefaultStream(&stream->pa_stream, 0, STREAM_CHANNEL_COUNT, paFloat32, STREAM_SAMPLE_RATE, paFramesPerBufferUnspecified, Callback, stream ) == paNoError)
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
	bool success = true;

	if (stream != NULL)
	{
		success = Pa_CloseStream(stream->pa_stream) == paNoError;
		free(stream);
	}

	return success;
}

bool Backend_SetVolume(BackendStream *stream, float volume)
{
	if (stream != NULL)
		stream->volume = volume * volume;

	return true;
}

bool Backend_PauseStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StopStream(stream->pa_stream) == paNoError;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StartStream(stream->pa_stream) == paNoError;

	return success;
}
