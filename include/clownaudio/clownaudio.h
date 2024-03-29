// Copyright (c) 2019-2021 Clownacy
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef CLOWNAUDIO_H
#define CLOWNAUDIO_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

#include "mixer.h"

#ifndef CLOWNAUDIO_EXPORT
#include "clownaudio_export.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Defined in `mixer.h`, but included here for documentation

typedef struct ClownAudio_SoundDataConfig
{
	// To 'predecode' means to decode sound data to raw PCM when it is loaded. This removes the overhead of decoding the sound data during playback.
	// The downside of this is that it causes sound data to take significantly longer to load. This may cause the thread you are loading the data on to stall.
	// If you use predecoding and load sound data on the main thread, then it may be best to load your sound data during a loading screen.
	/// If true, the sound *may* be predecoded if possible. If not, the sound will still be loaded, albeit not predecoded.
	bool predecode;
	/// If true, the sound *must* be predecoded if possible. If not, the function will fail.
	bool must_predecode;
	/// If sound is predecoded, then this needs to be true for `ClownAudio_SoundSetSampleRate` to work
	bool dynamic_sample_rate;
} ClownAudio_SoundDataConfig;

typedef struct ClownAudio_SoundConfig
{
	/// If true, the sound will loop indefinitely
	bool loop;
	/// If true, the sound will not be automatically destroyed once it finishes playing
	bool do_not_destroy_when_done;
	/// If sound is not predecoded, then this needs to be true for `ClownAudio_SoundSetSampleRate` to work
	bool dynamic_sample_rate;
} ClownAudio_SoundConfig;


//////////////////////////////////
// Configuration initialisation //
//////////////////////////////////

/// Initialises a `ClownAudio_SoundDataConfig` struct with sane default values
CLOWNAUDIO_EXPORT void ClownAudio_SoundDataConfigInit(ClownAudio_SoundDataConfig *config);

/// Initialises a `ClownAudio_SoundConfig` struct with sane default values
CLOWNAUDIO_EXPORT void ClownAudio_SoundConfigInit(ClownAudio_SoundConfig *config);
*/

////////////////////
// Initialisation //
////////////////////

/// Initialises clownaudio
CLOWNAUDIO_EXPORT bool ClownAudio_Init(void);

/// Deinitialises clownaudio
CLOWNAUDIO_EXPORT void ClownAudio_Deinit(void);


//////////////////////////////////
// Sound-data loading/unloading //
//////////////////////////////////

/// Loads data from up to two memory buffers - either buffer pointer can be NULL.
/// If two buffers are specified and looping is enabled, the sound will loop at the point where the first buffer ends and the second one begins.
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_SoundDataLoadFromMemory(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, ClownAudio_SoundDataConfig *config);

/// Loads data from up to two files - either file path can be NULL.
/// If two files are specified and looping is enabled, the sound will loop at the point where the first file ends and the second one begins.
CLOWNAUDIO_EXPORT ClownAudio_SoundData* ClownAudio_SoundDataLoadFromFiles(const char *intro_path, const char *loop_path, ClownAudio_SoundDataConfig *config);

/// Unloads data. All sounds using the specified data must be destroyed manually before this function is called.
CLOWNAUDIO_EXPORT void ClownAudio_SoundDataUnload(ClownAudio_SoundData *sound_data);


////////////////////////////////
// Sound creation/destruction //
////////////////////////////////

/// Creates a sound from sound-data. The sound will be paused by default.
CLOWNAUDIO_EXPORT ClownAudio_SoundID ClownAudio_SoundCreate(ClownAudio_SoundData *sound_data, ClownAudio_SoundConfig *config);

/// Destroys sound.
CLOWNAUDIO_EXPORT void ClownAudio_SoundDestroy(ClownAudio_SoundID sound_id);


/////////////////////////////
// Assorted sound controls //
/////////////////////////////

/// Rewinds sound to the very beginning.
CLOWNAUDIO_EXPORT void ClownAudio_SoundRewind(ClownAudio_SoundID sound_id);

/// Pauses sound.
CLOWNAUDIO_EXPORT void ClownAudio_SoundPause(ClownAudio_SoundID sound_id);

/// Unpauses sound.
CLOWNAUDIO_EXPORT void ClownAudio_SoundUnpause(ClownAudio_SoundID sound_id);

/// Returns -1 if the sound does not exist, 0 if it is unpaused, or 1 if it is paused.
CLOWNAUDIO_EXPORT int ClownAudio_SoundGetStatus(ClownAudio_SoundID sound_id);

/// Sets stereo volume. Volume is linear and ranges from 0 (silence) to 0x100 (full volume). Exceeding 0x100 will amplify the volume.
CLOWNAUDIO_EXPORT void ClownAudio_SoundSetVolume(ClownAudio_SoundID sound_id, unsigned short volume_left, unsigned short volume_right);

/// Change whether the sound should loop or not. Only certain file formats support this (for example, Ogg Vorbis does but PxTone doesn't).
CLOWNAUDIO_EXPORT void ClownAudio_SoundSetLoop(ClownAudio_SoundID sound_id, bool loop);

/// Sets the sound's speed. Full speed is 0x10000, half-speed is 0x8000, and double-speed is 0x20000.
/// Note: the sound must have been created with `dynamic_sample_rate` enabled in the configuration struct, otherwise this function might silently fail.
CLOWNAUDIO_EXPORT void ClownAudio_SoundSetSpeed(ClownAudio_SoundID sound_id, unsigned long speed);

/// Set the low-pass filter cut-off point. Remember that the frequency is half of the sample rate.
CLOWNAUDIO_EXPORT void ClownAudio_SoundSetLowPassFilter(ClownAudio_SoundID sound_id, unsigned long low_pass_filter_sample_rate);

/// Make sound fade to the specified volume over the specified duration, measured in milliseconds.
/// Volume is linear and ranges from 0 (silence) to 0x100 (full volume). Exceeding 0x100 will amplify the volume.
/// Note that this volume is separate from the one set with ClownAudio_SetSoundVolume, and in fact stacks with it.
CLOWNAUDIO_EXPORT void ClownAudio_SoundFade(ClownAudio_SoundID sound_id, unsigned short volume, unsigned int duration);


#ifdef __cplusplus
}
#endif

#endif // CLOWNAUDIO_H
