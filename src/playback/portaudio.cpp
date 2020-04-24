/*
 *  (C) 2018-2020 Clownacy
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

#include "clownaudio/playback.h"

#include <stddef.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#endif

#include "portaudio.h"

struct ClownAudio_Stream
{
	void (*user_callback)(void*, float*, size_t);
	void *user_data;

	PaStream *pa_stream;
};

typedef struct ClownAudio_Mutex
{
#ifdef _WIN32
	HANDLE handle;
#else
	pthread_mutex_t pthread_mutex;
#endif
} ClownAudio_Mutex;

static int Callback(const void *input_buffer, void *output_buffer_void, unsigned long frames_to_do, const PaStreamCallbackTimeInfo *time_info, PaStreamCallbackFlags status_flags, void *user_data)
{
	(void)input_buffer;
	(void)time_info;
	(void)status_flags;

	ClownAudio_Stream *stream = (ClownAudio_Stream*)user_data;
	float *output_buffer = (float*)output_buffer_void;

	stream->user_callback(stream->user_data, output_buffer, frames_to_do);

	return paContinue;
}

CLOWNAUDIO_EXPORT bool ClownAudio_InitPlayback(void)
{
	return Pa_Initialize() == paNoError;
}

CLOWNAUDIO_EXPORT void ClownAudio_DeinitPlayback(void)
{
	Pa_Terminate();
}

CLOWNAUDIO_EXPORT ClownAudio_Stream* ClownAudio_CreateStream(unsigned long *sample_rate, void (*user_callback)(void*, float*, size_t))
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)malloc(sizeof(ClownAudio_Stream));

	if (stream != NULL)
	{
		const PaDeviceInfo *device_info = Pa_GetDeviceInfo(Pa_GetDefaultOutputDevice());

		*sample_rate = device_info->defaultSampleRate;

		if (Pa_OpenDefaultStream(&stream->pa_stream, 0, CLOWNAUDIO_STREAM_CHANNEL_COUNT, paFloat32, device_info->defaultSampleRate, paFramesPerBufferUnspecified, Callback, stream ) == paNoError)
		{
			stream->user_callback = user_callback;
			stream->user_data = NULL;

			return stream;
		}

		free(stream);
	}

	return NULL;
}

CLOWNAUDIO_EXPORT bool ClownAudio_DestroyStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
	{
		success = Pa_CloseStream(stream->pa_stream) == paNoError;
		free(stream);
	}

	return success;
}

CLOWNAUDIO_EXPORT void ClownAudio_SetStreamCallbackData(ClownAudio_Stream *stream, void *user_data)
{
	if (stream != NULL)
		stream->user_data = user_data;
}

CLOWNAUDIO_EXPORT bool ClownAudio_PauseStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StopStream(stream->pa_stream) == paNoError;

	return success;
}

CLOWNAUDIO_EXPORT bool ClownAudio_ResumeStream(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
		success = Pa_StartStream(stream->pa_stream) == paNoError;

	return success;
}

CLOWNAUDIO_EXPORT ClownAudio_Mutex* ClownAudio_MutexInit(void)
{
	ClownAudio_Mutex *mutex = (ClownAudio_Mutex*)malloc(sizeof(ClownAudio_Mutex));

	if (mutex != NULL)
	{
	#ifdef _WIN32
		mutex->handle = CreateEventA(NULL, FALSE, TRUE, NULL);
	#else
		pthread_mutex_init(&mutex->pthread_mutex, NULL);
	#endif
	}

	return mutex;
}

CLOWNAUDIO_EXPORT void ClownAudio_MutexDeinit(ClownAudio_Mutex *mutex)
{
#ifdef _WIN32
	CloseHandle(mutex->handle);
#else
	pthread_mutex_destroy(&mutex->pthread_mutex);
#endif
}

CLOWNAUDIO_EXPORT void ClownAudio_MutexLock(ClownAudio_Mutex *mutex)
{
#ifdef _WIN32
	WaitForSingleObject(mutex->handle, INFINITE);
#else
	pthread_mutex_lock(&mutex->pthread_mutex);
#endif
}

CLOWNAUDIO_EXPORT void ClownAudio_MutexUnlock(ClownAudio_Mutex *mutex)
{
#ifdef _WIN32
	SetEvent(mutex->handle);
#else
	pthread_mutex_unlock(&mutex->pthread_mutex);
#endif
}
