#include "dr_wav.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO

#include "libs/dr_wav.h"

#include "common.h"

struct Decoder_DR_WAV
{
	drwav *instance;
	bool loops;
};

Decoder_DR_WAV* Decoder_DR_WAV_Create(DecoderData *data, bool loops, DecoderInfo *info)
{
	Decoder_DR_WAV *decoder = NULL;

	if (data != NULL)
	{
		drwav *instance = drwav_open_memory(data->file_buffer, data->file_size);

		if (instance != NULL)
		{
			decoder = malloc(sizeof(Decoder_DR_WAV));

			if (decoder != NULL)
			{
				decoder->instance = instance;
				decoder->loops = loops;

				info->sample_rate = instance->sampleRate;
				info->channel_count = instance->channels;
				info->format = DECODER_FORMAT_F32;
			}
			else
			{
				drwav_uninit(instance);
			}
		}
	}

	return decoder;
}

void Decoder_DR_WAV_Destroy(Decoder_DR_WAV *decoder)
{
	if (decoder != NULL)
	{
		drwav_uninit(decoder->instance);
		free(decoder->instance);
		free(decoder);
	}
}

void Decoder_DR_WAV_Rewind(Decoder_DR_WAV *decoder)
{
	drwav_seek_to_pcm_frame(decoder->instance, 0);
}

unsigned long Decoder_DR_WAV_GetSamples(Decoder_DR_WAV *decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		frames_done += (unsigned long)drwav_read_pcm_frames_f32(decoder->instance, frames_to_do - frames_done, &buffer[frames_done * decoder->instance->channels]);

		if (frames_done != frames_to_do && decoder->loops)
			Decoder_DR_WAV_Rewind(decoder);
		else
			break;
	}

	return frames_done;
}
