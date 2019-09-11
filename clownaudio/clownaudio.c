#include "clownaudio.h"

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

bool ClownAudio_Init(void)
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

ClownAudio_Sound* ClownAudio_LoadSound(const char *file_path, bool predecode)
{
	return (ClownAudio_Sound*)Mixer_LoadSound(file_path, predecode);
}

void ClownAudio_UnloadSound(ClownAudio_Sound *sound)
{
	Mixer_UnloadSound((Mixer_Sound*)sound);
}

ClownAudio_SoundInstanceID ClownAudio_PlaySound(ClownAudio_Sound *sound, bool loop)
{
	return Mixer_PlaySound((Mixer_Sound*)sound, loop);
}

void ClownAudio_StopSound(ClownAudio_SoundInstanceID instance)
{
	Mixer_StopSound(instance);
}

void ClownAudio_PauseSound(ClownAudio_SoundInstanceID instance)
{
	Mixer_PauseSound(instance);
}

void ClownAudio_UnpauseSound(ClownAudio_SoundInstanceID instance)
{
	Mixer_UnpauseSound(instance);
}

void ClownAudio_FadeOutSound(ClownAudio_SoundInstanceID instance, unsigned int duration)
{
	Mixer_FadeOutSound(instance, duration);
}

void ClownAudio_FadeInSound(ClownAudio_SoundInstanceID instance, unsigned int duration)
{
	Mixer_FadeInSound(instance, duration);
}

void ClownAudio_SetSoundVolume(ClownAudio_SoundInstanceID instance, float volume)
{
	Mixer_SetSoundVolume(instance, volume);
}
