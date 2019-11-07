#include "pxtone_noise.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libs/pxtone/shim.h"

#include "common.h"
#include "memory_stream.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct Decoder_PxToneNoise
{
	ROMemoryStream *memory_stream;
	bool loop;
};

Decoder_PxToneNoise* Decoder_PxToneNoise_Create(DecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	(void)sample_rate;	// PxTone-Noise only supports a few specific sample rates
	(void)channel_count;

	Decoder_PxToneNoise *decoder = NULL;

	if (data != NULL)
	{
		void *buffer;
		size_t buffer_size;

		if (PxTone_NoiseGenerate(data->file_buffer, data->file_size, SAMPLE_RATE, CHANNEL_COUNT, &buffer, &buffer_size))
		{
			ROMemoryStream *memory_stream = ROMemoryStream_Create(buffer, buffer_size);

			if (memory_stream != NULL)
			{
				decoder = malloc(sizeof(Decoder_PxToneNoise));

				if (decoder != NULL)
				{
					decoder->memory_stream = memory_stream;
					decoder->loop = loop;

					info->sample_rate = SAMPLE_RATE;
					info->channel_count = CHANNEL_COUNT;
					info->format = DECODER_FORMAT_S16;
				}
				else
				{
					ROMemoryStream_Destroy(memory_stream);
				}
			}
		}
	}

	return decoder;
}

void Decoder_PxToneNoise_Destroy(Decoder_PxToneNoise *decoder)
{
	if (decoder != NULL)
	{
		ROMemoryStream_Destroy(decoder->memory_stream);
		free(decoder);
	}
}

void Decoder_PxToneNoise_Rewind(Decoder_PxToneNoise *decoder)
{
	ROMemoryStream_Rewind(decoder->memory_stream);
}

unsigned long Decoder_PxToneNoise_GetSamples(Decoder_PxToneNoise *decoder, void *buffer_void, unsigned long frames_to_do)
{
	short *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = ROMemoryStream_Read(decoder->memory_stream, buffer + (frames_done_total * CHANNEL_COUNT), sizeof(short) * CHANNEL_COUNT, frames_to_do - frames_done_total);

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (decoder->loop)
				Decoder_PxToneNoise_Rewind(decoder);
			else
				break;
		}
	}

	return frames_done_total;
}
