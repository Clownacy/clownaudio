/*
 *  (C) 2019-2020 Clownacy
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

#include "clownaudio/clownaudio.h"

#include <stddef.h>

#include "clownaudio/mixer.h"
#include "clownaudio/playback.h"

static ClownAudio_Stream *stream;
static ClownAudio_Mixer *mixer;
static ClownAudio_Mutex *mutex;

static void StreamCallback(void *user_data, float *output_buffer, size_t frames_to_do)
{
	// Clear buffer (`ClownAudio_MixSamples` mixes directly with the output buffer)
	for (size_t i = 0; i < frames_to_do * CLOWNAUDIO_STREAM_CHANNEL_COUNT; ++i)
		output_buffer[i] = 0.0f;

	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_MixSamples((ClownAudio_Mixer*)user_data, output_buffer, frames_to_do);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT bool ClownAudio_Init(void)
{
	mutex = ClownAudio_MutexInit();

	if (mutex != NULL)
	{
		if (ClownAudio_InitPlayback())
		{
			unsigned long sample_rate = 48000;	// This default value is a fallback - it will be overwritten if the backend has a preferred rate
			stream = ClownAudio_CreateStream(&sample_rate, StreamCallback);

			if (stream != NULL)
			{
				mixer = ClownAudio_CreateMixer(sample_rate);

				if (mixer != NULL)
				{
					ClownAudio_SetStreamCallbackData(stream, mixer);

					ClownAudio_ResumeStream(stream);

					return true;
				}

				ClownAudio_DestroyStream(stream);
			}

			ClownAudio_DeinitPlayback();
		}

		ClownAudio_MutexDeinit(mutex);
	}

	return false;
}

CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void)
{
	ClownAudio_DestroyMixer(mixer);
	ClownAudio_DestroyStream(stream);
	ClownAudio_DeinitPlayback();
	ClownAudio_MutexDeinit(mutex);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_LoadSoundDataFromMemory(mixer, file_buffer1, file_size1, file_buffer2, file_size2, config);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_LoadSoundDataFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_LoadSoundDataFromFiles(mixer, intro_path, loop_path, config);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound_data)
{
	ClownAudio_Mixer_UnloadSoundData(sound_data);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_CreateSound(ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config)
{
	ClownAudio_Sound *sound = ClownAudio_Mixer_CreateSound(mixer, sound_data, config);

	ClownAudio_MutexLock(mutex);
	ClownAudio_SoundID sound_id = ClownAudio_Mixer_RegisterSound(mixer, sound);
	ClownAudio_MutexUnlock(mutex);

	return sound_id;
}

CLOWNAUDIO_EXPORT void ClownAudio_DestroySound(ClownAudio_SoundID sound_id)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_DestroySound(mixer, sound_id);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_RewindSound(ClownAudio_SoundID sound_id)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_RewindSound(mixer, sound_id);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_PauseSound(ClownAudio_SoundID sound_id)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_PauseSound(mixer, sound_id);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_UnpauseSound(ClownAudio_SoundID sound_id)
{	
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_UnpauseSound(mixer, sound_id);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeOutSound(ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_FadeOutSound(mixer, sound_id, duration);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_FadeInSound(ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_FadeInSound(mixer, sound_id, duration);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_CancelFade(ClownAudio_SoundID sound_id)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_CancelFade(mixer, sound_id);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT int ClownAudio_GetSoundStatus(ClownAudio_SoundID sound_id)
{
	ClownAudio_MutexLock(mutex);
	int status = ClownAudio_Mixer_GetSoundStatus(mixer, sound_id);
	ClownAudio_MutexUnlock(mutex);

	return status;
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundVolume(ClownAudio_SoundID sound_id, float volume_left, float volume_right)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_SetSoundVolume(mixer, sound_id, volume_left, volume_right);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundLoop(ClownAudio_SoundID sound_id, bool loop)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_SetSoundLoop(mixer, sound_id, loop);
	ClownAudio_MutexUnlock(mutex);
}

CLOWNAUDIO_EXPORT void ClownAudio_SetSoundSampleRate(ClownAudio_SoundID sound_id, unsigned long sample_rate)
{
	ClownAudio_MutexLock(mutex);
	ClownAudio_Mixer_SetSoundSampleRate(mixer, sound_id, sample_rate);
	ClownAudio_MutexUnlock(mutex);
}
