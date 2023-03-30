/*
Copyright (c) 2018-2023 Ned Loynd

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/

#include "clownaudio/playback.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdlib.h>

#include <pthread.h>

#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>

#include <AvailabilityMacros.h>

#if !defined(MAC_OS_X_VERSION_10_6) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_6
/* These deprecated Carbon APIs were replaced with equivalent functions and types in macOS 10.6 */
#include <CoreServices/CoreServices.h>
#define AudioComponent Component
#define AudioComponentDescription ComponentDescription
#define AudioComponentFindNext FindNextComponent
#define AudioComponentInstanceDispose CloseComponent
#define AudioComponentInstanceNew OpenAComponent
#endif

struct ClownAudio_Stream
{
	void (*user_callback)(void*, short*, size_t);
	void *user_data;

	AudioUnit audio_unit;

	pthread_mutex_t pthread_mutex;
};

static AudioComponent default_output_component;

static OSStatus Callback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)inRefCon;

	(void)ioActionFlags;
	(void)inTimeStamp;
	(void)inBusNumber;

	/* Because the stream is interleaved, there is only one buffer */
	stream->user_callback(stream->user_data, (short*)ioData->mBuffers[0].mData, inNumberFrames);

	return 0;
}

CLOWNAUDIO_EXPORT bool ClownAudio_InitPlayback(void)
{
	/* Find the default output AudioUnit */
	AudioComponentDescription default_output_description;
	bool success = true;

	default_output_description.componentType = kAudioUnitType_Output;
	default_output_description.componentSubType = kAudioUnitSubType_DefaultOutput;
	default_output_description.componentManufacturer = kAudioUnitManufacturer_Apple;
	default_output_description.componentFlags = 0;
	default_output_description.componentFlagsMask = 0;

	default_output_component = AudioComponentFindNext(NULL, &default_output_description);

	if (default_output_component == NULL)
		success = false;

	return success;
}

CLOWNAUDIO_EXPORT void ClownAudio_DeinitPlayback(void)
{
	default_output_component = NULL;
}

CLOWNAUDIO_EXPORT ClownAudio_Stream* ClownAudio_StreamCreate(unsigned long *sample_rate, void (*user_callback)(void *user_data, short *output_buffer, size_t frames_to_do))
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)malloc(sizeof(ClownAudio_Stream));

	if (stream != NULL)
	{
		if (default_output_component != NULL)
		{
			/* Create a new instance of the default output AudioUnit */
			OSStatus error = AudioComponentInstanceNew(default_output_component, &stream->audio_unit);

			if (!error)
			{
				/* Use a suitable output format */
				AudioStreamBasicDescription want;
				want.mFormatID = kAudioFormatLinearPCM;
				want.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked
			#if defined(__ppc64__) || defined(__ppc__)
				                    | kAudioFormatFlagIsBigEndian
			#endif
				                    ;
				/* TODO: Get default sample rate */
				want.mSampleRate = (float) *sample_rate;
				/* 16 bit output */
				want.mBitsPerChannel = sizeof(short) * 8;
				want.mChannelsPerFrame = CLOWNAUDIO_STREAM_CHANNEL_COUNT;
				/* kAudioFormatLinearPCM doesn't use packets */
				want.mFramesPerPacket = 1;
				/* Bytes per channel * channels per frame */
				want.mBytesPerFrame = (want.mBitsPerChannel / 8) * want.mChannelsPerFrame;
				/* Bytes per frame * frames per packet */
				want.mBytesPerPacket = want.mBytesPerFrame * want.mFramesPerPacket;

				error = AudioUnitSetProperty(stream->audio_unit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &want, sizeof(AudioStreamBasicDescription));

				if (!error)
				{
					/* Set the AudioUnit output callback */
					AURenderCallbackStruct callback;
					/* Callback function */
					callback.inputProc = Callback;
					/* User data */
					callback.inputProcRefCon = stream;

					error = AudioUnitSetProperty(stream->audio_unit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(AURenderCallbackStruct));

					if (!error)
					{
						/* Initialize the configured AudioUnit instance */
						error = AudioUnitInitialize(stream->audio_unit);

						if (!error)
						{
							stream->user_callback = user_callback;
							stream->user_data = NULL;

							pthread_mutex_init(&stream->pthread_mutex, NULL);

							return stream;
						}
					}
				}

				AudioComponentInstanceDispose(stream->audio_unit);
			}
		}

		free(stream);
	}

	return NULL;
}

CLOWNAUDIO_EXPORT bool ClownAudio_StreamDestroy(ClownAudio_Stream *stream)
{
	bool success = true;

	if (stream != NULL)
	{
		OSStatus error;
		success = false;

		error = AudioOutputUnitStop(stream->audio_unit);

		if (!error)
		{
			error = AudioUnitSetProperty(stream->audio_unit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, NULL, 0);

			if (!error)
			{
				error = AudioUnitUninitialize(stream->audio_unit);

				if (!error)
				{
					error = AudioComponentInstanceDispose(stream->audio_unit);

					if (!error)
					{
						pthread_mutex_destroy(&stream->pthread_mutex);

						free(stream);

						success = true;
					}
				}
			}
		}
	}

	return success;
}

CLOWNAUDIO_EXPORT void ClownAudio_StreamSetCallbackData(ClownAudio_Stream *stream, void *user_data)
{
	if (stream != NULL)
		stream->user_data = user_data;
}

CLOWNAUDIO_EXPORT bool ClownAudio_StreamPause(ClownAudio_Stream *stream)
{
	OSStatus error = 0;

	if (stream != NULL)
		error = AudioOutputUnitStop(stream->audio_unit);

	return !error;
}

CLOWNAUDIO_EXPORT bool ClownAudio_StreamResume(ClownAudio_Stream *stream)
{
	OSStatus error = 0;

	if (stream != NULL)
		error = AudioOutputUnitStart(stream->audio_unit);

	return !error;
}

CLOWNAUDIO_EXPORT void ClownAudio_StreamLock(ClownAudio_Stream *stream)
{
	if (stream != NULL)
		pthread_mutex_lock(&stream->pthread_mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_StreamUnlock(ClownAudio_Stream *stream)
{
	if (stream != NULL)
		pthread_mutex_unlock(&stream->pthread_mutex);
}
