#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct Mixer_SoundData Mixer_SoundData;
typedef unsigned int Mixer_Sound;

void Mixer_Init(unsigned long sample_rate, unsigned int channel_count);
void Mixer_Deinit(void);
Mixer_SoundData* Mixer_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2/*, bool predecode*/);
void Mixer_UnloadSoundData(Mixer_SoundData *sound);
Mixer_Sound Mixer_CreateSound(Mixer_SoundData *sound, bool loop);
void Mixer_DestroySound(Mixer_Sound instance);
void Mixer_PauseSound(Mixer_Sound instance);
void Mixer_UnpauseSound(Mixer_Sound instance);
void Mixer_FadeOutSound(Mixer_Sound instance, unsigned int duration);
void Mixer_FadeInSound(Mixer_Sound instance, unsigned int duration);
void Mixer_CancelFade(Mixer_Sound instance);
void Mixer_SetSoundVolume(Mixer_Sound instance, float volume);
void Mixer_MixSamples(float *output_buffer, unsigned long frames_to_do);
