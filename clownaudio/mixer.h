#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct Mixer_SoundData Mixer_SoundData;
typedef unsigned int Mixer_Sound;

void Mixer_Init(unsigned long sample_rate);
void Mixer_Deinit(void);

Mixer_SoundData* Mixer_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode);
void Mixer_UnloadSoundData(Mixer_SoundData *sound);

// If `free_when_done` is true, the sound will be destroyed once it finishes playing
Mixer_Sound Mixer_CreateSound(Mixer_SoundData *sound, bool loop, bool free_when_done);
void Mixer_DestroySound(Mixer_Sound instance);

void Mixer_RewindSound(Mixer_Sound instance);
void Mixer_PauseSound(Mixer_Sound instance);
void Mixer_UnpauseSound(Mixer_Sound instance);

void Mixer_FadeOutSound(Mixer_Sound instance, unsigned int duration);	// Duration is in milliseconds
void Mixer_FadeInSound(Mixer_Sound instance, unsigned int duration);
void Mixer_CancelFade(Mixer_Sound instance);

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused
int Mixer_GetSoundStatus(Mixer_Sound instance);

void Mixer_SetSoundVolume(Mixer_Sound instance, float volume);	// Volume is logarithmic, between 0.0f and 1.0f
void Mixer_SetSoundLoop(Mixer_Sound instance, bool loop);
void Mixer_SetSoundSampleRate(Mixer_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2);
void Mixer_SetPan(Mixer_Sound instance, float pan);	// -1.0f is full-left; 1.0f is full-right

void Mixer_MixSamples(float *output_buffer, size_t frames_to_do);
