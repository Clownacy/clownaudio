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

	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_OutputSamples(mixer, output_buffer, frames_to_do);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT bool ClownAudio_Init(void)
{
	if (ClownAudio_InitPlayback())
	{
		unsigned long sample_rate = 48000;	// This default value is a fallback - it will be overwritten if the backend has a preferred rate
		stream = ClownAudio_Stream_Create(&sample_rate, StreamCallback);

		if (stream != NULL)
		{
			mixer = ClownAudio_Mixer_Create(sample_rate);

			if (mixer != NULL)
			{
				ClownAudio_Stream_Resume(stream);

				return true;
			}

			ClownAudio_Stream_Destroy(stream);
		}

		ClownAudio_DeinitPlayback();
	}

	return false;
}

CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void)
{
	ClownAudio_Stream_Pause(stream);
	ClownAudio_Mixer_Destroy(mixer);
	ClownAudio_Stream_Destroy(stream);
	ClownAudio_DeinitPlayback();
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_SoundData_LoadFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_SoundData_LoadFromMemory(mixer, file_buffer1, file_size1, file_buffer2, file_size2, config);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_SoundData_LoadFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config)
{
	return ClownAudio_Mixer_SoundData_LoadFromFiles(mixer, intro_path, loop_path, config);
}

CLOWNAUDIO_EXPORT void ClownAudio_SoundData_Unload(ClownAudio_SoundData *sound_data)
{
	ClownAudio_Mixer_SoundData_Unload(mixer, sound_data);
}

CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_Sound_Create(ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config)
{
	ClownAudio_Sound *sound = ClownAudio_Mixer_Sound_Create(mixer, sound_data, config);

	ClownAudio_Stream_Lock(stream);
	ClownAudio_SoundID sound_id = ClownAudio_Mixer_Sound_Register(mixer, sound, sound_data);
	ClownAudio_Stream_Unlock(stream);

	return sound_id;
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_Destroy(ClownAudio_SoundID sound_id)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_Destroy(mixer, sound_id);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_Rewind(ClownAudio_SoundID sound_id)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_Rewind(mixer, sound_id);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_Pause(ClownAudio_SoundID sound_id)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_Pause(mixer, sound_id);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_Unpause(ClownAudio_SoundID sound_id)
{	
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_Unpause(mixer, sound_id);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_FadeOut(ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_FadeOut(mixer, sound_id, duration);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_FadeIn(ClownAudio_SoundID sound_id, unsigned int duration)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_FadeIn(mixer, sound_id, duration);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_CancelFade(ClownAudio_SoundID sound_id)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_CancelFade(mixer, sound_id);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT int ClownAudio_Sound_GetStatus(ClownAudio_SoundID sound_id)
{
	ClownAudio_Stream_Lock(stream);
	int status = ClownAudio_Mixer_Sound_GetStatus(mixer, sound_id);
	ClownAudio_Stream_Unlock(stream);

	return status;
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_SetVolume(ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_SetVolume(mixer, sound_id, volume_left, volume_right);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_SetLoop(ClownAudio_SoundID sound_id, bool loop)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_SetLoop(mixer, sound_id, loop);
	ClownAudio_Stream_Unlock(stream);
}

CLOWNAUDIO_EXPORT void ClownAudio_Sound_SetSpeed(ClownAudio_SoundID sound_id, unsigned long speed)
{
	ClownAudio_Stream_Lock(stream);
	ClownAudio_Mixer_Sound_SetSpeed(mixer, sound_id, speed);
	ClownAudio_Stream_Unlock(stream);
}
