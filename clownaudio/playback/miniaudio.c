#include "playback.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../miniaudio.h"

struct BackendStream
{
	void (*user_callback)(void*, float*, unsigned long);
	void *user_data;

	ma_device device;
	float volume;
};

static void Callback(ma_device *device, void *output_buffer_void, const void *input_buffer, ma_uint32 frames_to_do)
{
	(void)input_buffer;

	BackendStream *stream = device->pUserData;
	float *output_buffer = output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	// Handle volume in software, since mini_al's API doesn't have volume control
	if (stream->volume != 1.0f)
		for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
			output_buffer[i] *= stream->volume;
}

bool Backend_Init(void)
{
	return true;
}

void Backend_Deinit(void)
{
	
}

BackendStream* Backend_CreateStream(void (*user_callback)(void*, float*, unsigned long), void *user_data)
{
	BackendStream *stream = malloc(sizeof(BackendStream));

	if (stream != NULL)
	{
		ma_device_config config = ma_device_config_init(ma_device_type_playback);
		config.playback.pDeviceID = NULL;
		config.playback.format = ma_format_f32;
		config.playback.channels = STREAM_CHANNEL_COUNT;
		config.sampleRate = STREAM_SAMPLE_RATE;
		config.noPreZeroedOutputBuffer = MA_TRUE;
		config.dataCallback = Callback;
		config.pUserData = stream;

		if (ma_device_init(NULL, &config, &stream->device) == MA_SUCCESS)
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
		ma_device_uninit(&stream->device);
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
	bool success = true;

	if (stream != NULL && ma_device_is_started(&stream->device))
		success = ma_device_stop(&stream->device) == MA_SUCCESS;

	return success;
}

bool Backend_ResumeStream(BackendStream *stream)
{
	bool success = true;

	if (stream != NULL && !ma_device_is_started(&stream->device))
		success = ma_device_start(&stream->device) == MA_SUCCESS;

	return success;
}
