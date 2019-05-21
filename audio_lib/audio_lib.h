#pragma once

#include <stdbool.h>

typedef struct AudioLib_Sound AudioLib_Sound;
typedef unsigned int AudioLib_SoundInstanceID;

bool AudioLib_Init(void);
void AudioLib_Deinit(void);
void AudioLib_Pause(void);
void AudioLib_Unpause(void);
AudioLib_Sound* AudioLib_LoadSound(const char *file_path, bool predecode);
void AudioLib_UnloadSound(AudioLib_Sound *sound);
AudioLib_SoundInstanceID AudioLib_PlaySound(AudioLib_Sound *sound, bool loop);
void AudioLib_StopSound(AudioLib_SoundInstanceID instance);
void AudioLib_PauseSound(AudioLib_SoundInstanceID instance);
void AudioLib_UnpauseSound(AudioLib_SoundInstanceID instance);
void AudioLib_FadeOutSound(AudioLib_SoundInstanceID instance, unsigned int duration);
void AudioLib_FadeInSound(AudioLib_SoundInstanceID instance, unsigned int duration);
void AudioLib_SetSoundVolume(AudioLib_SoundInstanceID instance, float volume);
