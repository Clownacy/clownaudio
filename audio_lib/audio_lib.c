#include "audio_lib.h"

#include <stdbool.h>
#include <stddef.h>

#include "mixer.h"
#include "playback.h"

static BackendStream *stream;

static void CallbackStream(void *user_data, float *output_buffer, unsigned long frames_to_do)
{
	(void)user_data;

	for (unsigned long i = 0; i < frames_to_do * STREAM_CHANNEL_COUNT; ++i)
		output_buffer[i] = 0.0f;

	Mixer_MixSamples(output_buffer, frames_to_do);
}

bool AudioLib_Init(void)
{
	bool success = false;

	if (Backend_Init())
	{
		stream = Backend_CreateStream(CallbackStream, NULL);

		if (stream)
		{
			Mixer_Init(STREAM_SAMPLE_RATE, STREAM_CHANNEL_COUNT);

			if (Backend_ResumeStream(stream))
				success = true;
			else
				AudioLib_Deinit();
		}
		else
		{
			Backend_Deinit();
		}
	}

	return success;
}

void AudioLib_Deinit(void)
{
	Backend_DestroyStream(stream);
	Backend_Deinit();
	Mixer_Deinit();
}

void AudioLib_Pause(void)
{
	Backend_PauseStream(stream);
}

void AudioLib_Unpause(void)
{
	Backend_ResumeStream(stream);
}

AudioLib_Sound* AudioLib_LoadSound(const char *file_path, bool predecode)
{
	return (AudioLib_Sound*)Mixer_LoadSound(file_path, predecode);
}

void AudioLib_UnloadSound(AudioLib_Sound *sound)
{
	Mixer_UnloadSound((Mixer_Sound*)sound);
}

AudioLib_SoundInstanceID AudioLib_PlaySound(AudioLib_Sound *sound, bool loop)
{
	return Mixer_PlaySound((Mixer_Sound*)sound, loop);
}

void AudioLib_StopSound(AudioLib_SoundInstanceID instance)
{
	Mixer_StopSound(instance);
}

void AudioLib_PauseSound(AudioLib_SoundInstanceID instance)
{
	Mixer_PauseSound(instance);
}

void AudioLib_UnpauseSound(AudioLib_SoundInstanceID instance)
{
	Mixer_UnpauseSound(instance);
}

void AudioLib_FadeOutSound(AudioLib_SoundInstanceID instance, unsigned int duration)
{
	Mixer_FadeOutSound(instance, duration);
}

void AudioLib_FadeInSound(AudioLib_SoundInstanceID instance, unsigned int duration)
{
	Mixer_FadeInSound(instance, duration);
}

void AudioLib_SetSoundVolume(AudioLib_SoundInstanceID instance, float volume)
{
	Mixer_SetSoundVolume(instance, volume);
}
