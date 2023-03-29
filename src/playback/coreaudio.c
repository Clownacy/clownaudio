// Copyright (c) 2018-2023 Clownacy
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
// These deprecated Carbon APIs were replaced with equivalent functions and types in macOS 10.6
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

	AudioUnit audioUnit;

	pthread_mutex_t pthread_mutex;
};

static OSStatus Callback(void *inRefCon, AudioUnitRenderActionFlags *ioActionFlags, const AudioTimeStamp *inTimeStamp, UInt32 inBusNumber, UInt32 inNumberFrames, AudioBufferList *ioData)
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)inRefCon;
	// Because the stream is interleaved, there is only one buffer
	stream->user_callback(stream->user_data, (short*)ioData->mBuffers[0].mData, inNumberFrames);
	return 0;
}

CLOWNAUDIO_EXPORT bool ClownAudio_InitPlayback(void)
{
	// TODO: Should anything be here?
	return true;
}

CLOWNAUDIO_EXPORT void ClownAudio_DeinitPlayback(void)
{
	// TODO: Should anything be here?
}

CLOWNAUDIO_EXPORT ClownAudio_Stream* ClownAudio_StreamCreate(unsigned long *sample_rate, void (*user_callback)(void *user_data, short *output_buffer, size_t frames_to_do))
{
	ClownAudio_Stream *stream = (ClownAudio_Stream*)malloc(sizeof(ClownAudio_Stream));

	if (stream != NULL)
	{
		// Find the default output AudioUnit
		AudioComponentDescription defaultOutputDescription;
		AudioComponent defaultOutputComponent;
		defaultOutputDescription.componentType = kAudioUnitType_Output;
		defaultOutputDescription.componentSubType = kAudioUnitSubType_DefaultOutput;
		defaultOutputDescription.componentManufacturer = kAudioUnitManufacturer_Apple;
		defaultOutputDescription.componentFlags = 0;
		defaultOutputDescription.componentFlagsMask = 0;

		defaultOutputComponent = AudioComponentFindNext(NULL, &defaultOutputDescription);
		if (defaultOutputComponent != NULL)
		{
			// Create a new instance of the default output AudioUnit
			OSStatus error = AudioComponentInstanceNew(defaultOutputComponent, &stream->audioUnit);
			if (!error)
			{
				// Use a suitable output format
				AudioStreamBasicDescription want;
				want.mFormatID = kAudioFormatLinearPCM;
				want.mFormatFlags = kLinearPCMFormatFlagIsSignedInteger | kLinearPCMFormatFlagIsPacked
			#if !defined(MAC_OS_X_VERSION_10_2) || MAC_OS_X_VERSION_MIN_REQUIRED < MAC_OS_X_VERSION_10_2
				                    | kAudioFormatFlagsNativeEndian
			#endif
				                    ;
				// TODO: Get default sample rate
				want.mSampleRate = (float) *sample_rate;
				// 16 bit output
				want.mBitsPerChannel = sizeof(short) * 8;
				want.mChannelsPerFrame = CLOWNAUDIO_STREAM_CHANNEL_COUNT;
				// kAudioFormatLinearPCM doesn't use packets
				want.mFramesPerPacket = 1;
				// Bytes per channel * channels per frame
				want.mBytesPerFrame = (want.mBitsPerChannel / 8) * want.mChannelsPerFrame;
				// Bytes per frame * frames per packet
				want.mBytesPerPacket = want.mBytesPerFrame * want.mFramesPerPacket;

				error = AudioUnitSetProperty(stream->audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &want, sizeof(AudioStreamBasicDescription));
				if (!error)
				{
					// Set the AudioUnit output callback
					AURenderCallbackStruct callback;
					// Callback function
					callback.inputProc = Callback;
					// User data
					callback.inputProcRefCon = stream;
					error = AudioUnitSetProperty(stream->audioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &callback, sizeof(AURenderCallbackStruct));
					if (!error)
					{
						// Initialize the configured AudioUnit instance
						error = AudioUnitInitialize(stream->audioUnit);
						if (!error)
						{
							stream->user_callback = user_callback;
							stream->user_data = NULL;

							pthread_mutex_init(&stream->pthread_mutex, NULL);

							return stream;
						}
					}
				}
				AudioComponentInstanceDispose(stream->audioUnit);
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

		error = AudioOutputUnitStop(stream->audioUnit);
		if (!error)
		{
			error = AudioUnitUninitialize(stream->audioUnit);
			if (!error)
			{
				error = AudioComponentInstanceDispose(stream->audioUnit);
				if (!error)
				{
					pthread_mutex_destroy(&stream->pthread_mutex);

					free(stream);

					success = true;
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
	OSStatus error = AudioOutputUnitStop(stream->audioUnit);
	return !error;
}

CLOWNAUDIO_EXPORT bool ClownAudio_StreamResume(ClownAudio_Stream *stream)
{
	OSStatus error = AudioOutputUnitStart(stream->audioUnit);
	return !error;
}

CLOWNAUDIO_EXPORT void ClownAudio_StreamLock(ClownAudio_Stream *stream)
{
	if (stream != NULL)
	{
		pthread_mutex_lock(&stream->pthread_mutex);
	}
}

CLOWNAUDIO_EXPORT void ClownAudio_StreamUnlock(ClownAudio_Stream *stream)
{
	if (stream != NULL)
	{
		pthread_mutex_unlock(&stream->pthread_mutex);
	}
}
