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

bool ClownAudio_Init(void)
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

void ClownAudio_Deinit(void)
{
	Backend_DestroyStream(stream);
	Backend_Deinit();
	Mixer_Deinit();
}

void ClownAudio_Pause(void)
{
	Backend_PauseStream(stream);
}

void ClownAudio_Unpause(void)
{
	Backend_ResumeStream(stream);
}

ClownAudio_SoundData* ClownAudio_LoadSoundData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode)
{
	return (ClownAudio_SoundData*)Mixer_LoadSoundData(file_buffer1, file_size1, file_buffer2, file_size2, predecode);
}

void ClownAudio_UnloadSoundData(ClownAudio_SoundData *sound)
{
	Mixer_UnloadSoundData((Mixer_SoundData*)sound);
}

ClownAudio_Sound ClownAudio_CreateSound(ClownAudio_SoundData *sound, bool loop, bool free_when_done)
{
	return Mixer_CreateSound((Mixer_SoundData*)sound, loop, free_when_done);
}

void ClownAudio_DestroySound(ClownAudio_Sound instance)
{
	Mixer_DestroySound(instance);
}

void ClownAudio_RewindSound(ClownAudio_Sound instance)
{
	Mixer_RewindSound(instance);
}

void ClownAudio_PauseSound(ClownAudio_Sound instance)
{
	Mixer_PauseSound(instance);
}

void ClownAudio_UnpauseSound(ClownAudio_Sound instance)
{
	Mixer_UnpauseSound(instance);
}

void ClownAudio_FadeOutSound(ClownAudio_Sound instance, unsigned int duration)
{
	Mixer_FadeOutSound(instance, duration);
}

void ClownAudio_FadeInSound(ClownAudio_Sound instance, unsigned int duration)
{
	Mixer_FadeInSound(instance, duration);
}

int ClownAudio_GetSoundStatus(ClownAudio_Sound instance)
{
	return Mixer_GetSoundStatus(instance);
}

void ClownAudio_SetSoundVolume(ClownAudio_Sound instance, float volume)
{
	Mixer_SetSoundVolume(instance, volume);
}

void ClownAudio_SetSoundLoop(ClownAudio_Sound instance, bool loop)
{
	Mixer_SetSoundLoop(instance, loop);
}

void ClownAudio_SetSoundSampleRate(ClownAudio_Sound instance, unsigned long sample_rate1, unsigned long sample_rate2)
{
	Mixer_SetSoundSampleRate(instance, sample_rate1, sample_rate2);
}

void ClownAudio_SetSoundPan(ClownAudio_Sound instance, float pan)
{
	Mixer_SetPan(instance, pan);
}
