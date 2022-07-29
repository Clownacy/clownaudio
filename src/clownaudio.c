// Copyright (c) 2019-2021 Clownacy
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

#include "clownaudio/clownaudio.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>
#include <string.h>

#include "clownaudio/mixer.h"
#include "clownaudio/playback.h"

static ClownAudio_Stream *stream;
static ClownAudio_Mixer *mixer;

static void StreamCallback(void *user_data, short *output_buffer, size_t frames_to_do)
{
	(void)user_data;

	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_OutputSamples(mixer, output_buffer, frames_to_do);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT bool ClownAudio_Init(void)
{
	if (ClownAudio_InitPlayback())
	{
		unsigned long sample_rate = 48000;	// This default value is a fallback - it will be overwritten if the backend has a preferred rate
		stream = ClownAudio_StreamCreate(&sample_rate, StreamCallback);

		if (stream != NULL)
		{
			mixer = ClownAudio_Mixer_Create(sample_rate);

			if (mixer != NULL)
			{
				ClownAudio_StreamResume(stream);

				return true;
			}

			ClownAudio_StreamDestroy(stream);
		}

		ClownAudio_DeinitPlayback();
	}

	return false;
}

CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void)
{
	ClownAudio_StreamPause(stream);
	ClownAudio_Mixer_Destroy(mixer);
	ClownAudio_StreamDestroy(stream);
	ClownAudio_DeinitPlayback();
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_SoundDataLoadFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_SoundDataLoadFromMemory(mixer, file_buffer1, file_size1, file_buffer2, file_size2, config);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_SoundDataLoadFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_SoundDataLoadFromFiles(mixer, intro_path, loop_path, config);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundDataUnload(ClownAudio_SoundData *sound_data)
{
	ClownAudio_Mixer_SoundDataUnload(mixer, sound_data);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_SoundCreate(ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config)
{
	ClownAudio_Sound *sound = ClownAudio_Mixer_SoundCreate(mixer, sound_data, config);

	ClownAudio_StreamLock(stream);
	ClownAudio_SoundID sound_id = ClownAudio_Mixer_SoundRegister(mixer, sound, sound_data);
	ClownAudio_StreamUnlock(stream);

	return sound_id;
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundDestroy(ClownAudio_SoundID sound_id)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundDestroy(mixer, sound_id);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundRewind(ClownAudio_SoundID sound_id)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundRewind(mixer, sound_id);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundPause(ClownAudio_SoundID sound_id)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundPause(mixer, sound_id);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundUnpause(ClownAudio_SoundID sound_id)
{	
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundUnpause(mixer, sound_id);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundFade(ClownAudio_SoundID sound_id, unsigned short volume, unsigned int duration)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundFade(mixer, sound_id, volume, duration);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT int ClownAudio_SoundGetStatus(ClownAudio_SoundID sound_id)
{
	ClownAudio_StreamLock(stream);
	int status = ClownAudio_Mixer_SoundGetStatus(mixer, sound_id);
	ClownAudio_StreamUnlock(stream);

	return status;
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundSetVolume(ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundSetVolume(mixer, sound_id, volume_left, volume_right);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundSetLoop(ClownAudio_SoundID sound_id, bool loop)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundSetLoop(mixer, sound_id, loop);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundSetSpeed(ClownAudio_SoundID sound_id, unsigned long speed)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundSetSpeed(mixer, sound_id, speed);
	ClownAudio_StreamUnlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundSetLowPassFilter(ClownAudio_SoundID sound_id, unsigned long low_pass_filter_sample_rate)
{
	ClownAudio_StreamLock(stream);
	ClownAudio_Mixer_SoundSetLowPassFilter(mixer, sound_id, low_pass_filter_sample_rate);
	ClownAudio_StreamUnlock(stream);
}
