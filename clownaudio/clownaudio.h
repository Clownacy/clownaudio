#pragma once

#include <stdbool.h>

typedef struct ClownAudio_Sound ClownAudio_Sound;
typedef unsigned int ClownAudio_SoundInstanceID;

bool ClownAudio_Init(void);
void ClownAudio_Deinit(void);
void ClownAudio_Pause(void);
void ClownAudio_Unpause(void);
ClownAudio_Sound* ClownAudio_LoadSound(const char *file_path, bool predecode);
void ClownAudio_UnloadSound(ClownAudio_Sound *sound);
ClownAudio_SoundInstanceID ClownAudio_PlaySound(ClownAudio_Sound *sound, bool loop);
void ClownAudio_StopSound(ClownAudio_SoundInstanceID instance);
void ClownAudio_PauseSound(ClownAudio_SoundInstanceID instance);
void ClownAudio_UnpauseSound(ClownAudio_SoundInstanceID instance);
void ClownAudio_FadeOutSound(ClownAudio_SoundInstanceID instance, unsigned int duration);
void ClownAudio_FadeInSound(ClownAudio_SoundInstanceID instance, unsigned int duration);
void ClownAudio_SetSoundVolume(ClownAudio_SoundInstanceID instance, float volume);
