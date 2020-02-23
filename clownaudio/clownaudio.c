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

#include "clownaudio.h"

#include <stdbool.h>
#include <stddef.h>

#include "mixer.h"
#include "playback/playback.h"

static BackendStream *stream;

static void CallbackStream(void *user_data, float *output_buffer, size_t frames_to_do)
{
	(void)user_data;

	for (size_t i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
		output_buffer[i] = 0.0f;

	Mixer_MixSamples(output_buffer, frames_to_do);
}

DLL_API bool ClownAudio_Init(void)
{
	bool success = false;

	if (Backend_Init())
	{
		stream = Backend_CreateStream(CallbackStream, NULL);

		if (stream != NULL)
		{
			Mixer_Init(STREAM_SAMPLE_RATE);

			if (Backend_ResumeStream(stream))
				success = true;
			else
				ClownAudio_Deinit();
		}
		else
		{
			Backend_Deinit();
		}
	}

	return success;
}

DLL_API void ClownAudio_Deinit(void)
{
	Backend_DestroyStream(stream);
	Backend_Deinit();
	Mixer_Deinit();
}

DLL_API void ClownAudio_Pause(void)
{
	Backend_PauseStream(stream);
}

DLL_API void ClownAudio_Unpause(void)
{
	Backend_ResumeStream(stream);
}

DLL_API ClownAudio_SoundData* ClownAudio_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode)
{
	return (ClownAudio_SoundData*)Mixer_LoadSoundData(file_buffer1, file_size1, file_buffer2, file_size2, predecode);
}

DLL_API void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound)
{
	Mixer_UnloadSoundData((Mixer_SoundData*)sound);
}

DLL_API ClownAudio_Sound ClownAudio_CreateSound(ClownAudio_SoundData *sound, bool loop, bool free_when_done)
{
	return Mixer_CreateSound((Mixer_SoundData*)sound, loop, free_when_done);
}

DLL_API void ClownAudio_DestroySound(ClownAudio_Sound instance)
{
	Mixer_DestroySound(instance);
}

DLL_API void ClownAudio_RewindSound(ClownAudio_Sound instance)
{
	Mixer_RewindSound(instance);
}

DLL_API void ClownAudio_PauseSound(ClownAudio_Sound instance)
{
	Mixer_PauseSound(instance);
}

DLL_API void ClownAudio_UnpauseSound(ClownAudio_Sound instance)
{
	Mixer_UnpauseSound(instance);
}

DLL_API void ClownAudio_FadeOutSound(ClownAudio_Sound instance, unsigned int duration)
{
	Mixer_FadeOutSound(instance, duration);
}

DLL_API void ClownAudio_FadeInSound(ClownAudio_Sound instance, unsigned int duration)
{
	Mixer_FadeInSound(instance, duration);
}

DLL_API void ClownAudio_CancelFade(ClownAudio_Sound instance)
{
	Mixer_CancelFade(instance);
}

DLL_API int ClownAudio_GetSoundStatus(ClownAudio_Sound instance)
{
	return Mixer_GetSoundStatus(instance);
}

DLL_API void ClownAudio_SetSoundVolume(ClownAudio_Sound instance, float volume)
{
	Mixer_SetSoundVolume(instance, volume);
}

DLL_API void ClownAudio_SetSoundLoop(ClownAudio_Sound instance, bool loop)
{
	Mixer_SetSoundLoop(instance, loop);
}

DLL_API void ClownAudio_SetSoundSampleRate(ClownAudio_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2)
{
	Mixer_SetSoundSampleRate(instance, sample_rate1, sample_rate2);
}

DLL_API void ClownAudio_SetSoundPan(ClownAudio_Sound instance, float pan)
{
	Mixer_SetPan(instance, pan);
}
