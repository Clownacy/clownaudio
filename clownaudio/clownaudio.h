#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ClownAudio_SoundData ClownAudio_SoundData;
typedef unsigned int ClownAudio_Sound;

bool ClownAudio_Init(void);
void ClownAudio_Deinit(void);

void ClownAudio_Pause(void);
void ClownAudio_Unpause(void);

ClownAudio_SoundData* ClownAudio_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode);
void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound);

// If `free_when_done` is true, the sound will be destroyed once it finishes playing
ClownAudio_Sound ClownAudio_CreateSound(ClownAudio_SoundData *sound, bool loop, bool free_when_done);
void ClownAudio_DestroySound(ClownAudio_Sound instance);

void ClownAudio_RewindSound(ClownAudio_Sound instance);

void ClownAudio_PauseSound(ClownAudio_Sound instance);
void ClownAudio_UnpauseSound(ClownAudio_Sound instance);

void ClownAudio_FadeOutSound(ClownAudio_Sound instance, unsigned int duration);	// Duration is in milliseconds
void ClownAudio_FadeInSound(ClownAudio_Sound instance, unsigned int duration);
void ClownAudio_CancelFade(ClownAudio_Sound instance);

// Returns -1 if the sound doesn't exist, 0 if it's unpaused, or 1 if it is paused
int ClownAudio_GetSoundStatus(ClownAudio_Sound instance);

void ClownAudio_SetSoundVolume(ClownAudio_Sound instance, float volume);
void ClownAudio_SetSoundLoop(ClownAudio_Sound instance, bool loop);
void ClownAudio_SetSoundSampleRate(ClownAudio_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2);
void ClownAudio_SetSoundPan(ClownAudio_Sound instance, float pan);	// -1.0f is full-left; 1.0f is full-right

#ifdef __cplusplus
}
#endif
