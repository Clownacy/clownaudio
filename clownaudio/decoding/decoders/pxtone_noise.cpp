#include "pxtone_noise.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "libs/pxtone/pxtoneNoise.h"

#include "common.h"
#include "memory_stream.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct Decoder
{
	ROMemoryStream *memory_stream;
	void *buffer;
};

Decoder* Decoder_PxToneNoise_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	(void)loop;	// This is ignored in simple decoders

	Decoder *decoder = NULL;

	pxtoneNoise *pxtn = new pxtoneNoise();

	if (pxtn->init())
	{
		if (pxtn->quality_set(CHANNEL_COUNT, SAMPLE_RATE, 16))
		{
			pxtnDescriptor desc;

			if (desc.set_memory_r((void*)data, data_size))
			{
				void *buffer;
				int32_t buffer_size;

				if (pxtn->generate(&desc, &buffer, &buffer_size))
				{
					ROMemoryStream *memory_stream = ROMemoryStream_Create(buffer, buffer_size);

					if (memory_stream != NULL)
					{
						decoder = (Decoder*)malloc(sizeof(Decoder));

						if (decoder != NULL)
						{
							decoder->memory_stream = memory_stream;
							decoder->buffer = buffer;

							info->sample_rate = SAMPLE_RATE;
							info->channel_count = CHANNEL_COUNT;
							info->format = DECODER_FORMAT_S16;	// PxTone uses int16_t internally
							info->complex = false;

							delete pxtn;
							return decoder;
						}

						ROMemoryStream_Destroy(memory_stream);
					}

					free(buffer);
				}
			}
		}
	}

	delete pxtn;

	return decoder;
}

void Decoder_PxToneNoise_Destroy(Decoder *decoder)
{
	ROMemoryStream_Destroy(decoder->memory_stream);
	free(decoder->buffer);
	free(decoder);
}

void Decoder_PxToneNoise_Rewind(Decoder *decoder)
{
	ROMemoryStream_Rewind(decoder->memory_stream);
}

size_t Decoder_PxToneNoise_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do)
{
	return ROMemoryStream_Read(decoder->memory_stream, buffer, sizeof(int16_t) * CHANNEL_COUNT, frames_to_do);
}
