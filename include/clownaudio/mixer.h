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

typedef struct Mixer Mixer;
typedef struct Mixer_SoundData Mixer_SoundData;
typedef unsigned int Mixer_Sound;

DLL_API Mixer* Mixer_Create(unsigned long sample_rate);
DLL_API void Mixer_Destroy(Mixer *mixer);

DLL_API Mixer_SoundData* Mixer_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode);
DLL_API void Mixer_UnloadSoundData(Mixer_SoundData *sound);

// If `free_when_done` is true, the sound will be destroyed once it finishes playing
DLL_API Mixer_Sound Mixer_CreateSound(Mixer *mixer, Mixer_SoundData *sound, bool loop, bool free_when_done);
DLL_API void Mixer_DestroySound(Mixer *mixer, Mixer_Sound instance);

DLL_API void Mixer_RewindSound(Mixer *mixer, Mixer_Sound instance);
DLL_API void Mixer_PauseSound(Mixer *mixer, Mixer_Sound instance);
DLL_API void Mixer_UnpauseSound(Mixer *mixer, Mixer_Sound instance);

DLL_API void Mixer_FadeOutSound(Mixer *mixer, Mixer_Sound instance, unsigned int duration);	// Duration is in milliseconds
DLL_API void Mixer_FadeInSound(Mixer *mixer, Mixer_Sound instance, unsigned int duration);
DLL_API void Mixer_CancelFade(Mixer *mixer, Mixer_Sound instance);

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused
DLL_API int Mixer_GetSoundStatus(Mixer *mixer, Mixer_Sound instance);

DLL_API void Mixer_SetSoundVolume(Mixer *mixer, Mixer_Sound instance, float volume);	// Volume is logarithmic, between 0.0f and 1.0f
DLL_API void Mixer_SetSoundLoop(Mixer *mixer, Mixer_Sound instance, bool loop);
DLL_API void Mixer_SetSoundSampleRate(Mixer *mixer, Mixer_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2);
DLL_API void Mixer_SetPan(Mixer *mixer, Mixer_Sound instance, float pan);	// -1.0f is full-left; 1.0f is full-right

DLL_API void Mixer_MixSamples(Mixer *mixer, float *output_buffer, size_t frames_to_do);

#ifdef __cplusplus
}
#endif
