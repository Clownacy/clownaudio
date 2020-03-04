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

#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef BUILDING_DLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ClownMixer ClownMixer;
typedef struct ClownMixer_SoundData ClownMixer_SoundData;
typedef unsigned int ClownMixer_Sound;

DLL_API ClownMixer* ClownMixer_Create(unsigned long sample_rate);
DLL_API void ClownMixer_Destroy(ClownMixer *mixer);

DLL_API ClownMixer_SoundData* ClownMixer_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode);
DLL_API void ClownMixer_UnloadSoundData(ClownMixer_SoundData *sound);

// If `free_when_done` is true, the sound will be destroyed once it finishes playing
DLL_API ClownMixer_Sound ClownMixer_CreateSound(ClownMixer *mixer, ClownMixer_SoundData *sound, bool loop, bool free_when_done);
DLL_API void ClownMixer_DestroySound(ClownMixer *mixer, ClownMixer_Sound instance);

DLL_API void ClownMixer_RewindSound(ClownMixer *mixer, ClownMixer_Sound instance);
DLL_API void ClownMixer_PauseSound(ClownMixer *mixer, ClownMixer_Sound instance);
DLL_API void ClownMixer_UnpauseSound(ClownMixer *mixer, ClownMixer_Sound instance);

DLL_API void ClownMixer_FadeOutSound(ClownMixer *mixer, ClownMixer_Sound instance, unsigned int duration);	// Duration is in milliseconds
DLL_API void ClownMixer_FadeInSound(ClownMixer *mixer, ClownMixer_Sound instance, unsigned int duration);
DLL_API void ClownMixer_CancelFade(ClownMixer *mixer, ClownMixer_Sound instance);

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused
DLL_API int ClownMixer_GetSoundStatus(ClownMixer *mixer, ClownMixer_Sound instance);

DLL_API void ClownMixer_SetSoundVolume(ClownMixer *mixer, ClownMixer_Sound instance, float volume_left, float volume_right);	// Volume is linear, between 0.0f and 1.0f
DLL_API void ClownMixer_SetSoundLoop(ClownMixer *mixer, ClownMixer_Sound instance, bool loop);
DLL_API void ClownMixer_SetSoundSampleRate(ClownMixer *mixer, ClownMixer_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2);

DLL_API void ClownMixer_MixSamples(ClownMixer *mixer, float *output_buffer, size_t frames_to_do);

#ifdef __cplusplus
}
#endif
