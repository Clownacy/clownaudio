#include "pxtone_noise.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "libs/pxtone/pxtoneNoise.h"

#include "common.h"
#include "memory_stream.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct Decoder_PxToneNoise
{
	ROMemoryStream *memory_stream;
	bool loop;
};

Decoder_PxToneNoise* Decoder_PxToneNoise_Create(DecoderData *data, bool loop, DecoderInfo *info)
{
	Decoder_PxToneNoise *decoder = NULL;

	if (data != NULL)
	{
		pxtoneNoise *pxtn = new pxtoneNoise();

		if (pxtn->init())
		{
			if (pxtn->quality_set(CHANNEL_COUNT, SAMPLE_RATE, 16))
			{
				pxtnDescriptor desc;

				if (desc.set_memory_r((void*)data->file_buffer, data->file_size))
				{
					void *buffer;
					int32_t buffer_size;

					if (pxtn->generate(&desc, &buffer, &buffer_size))
					{
						ROMemoryStream *memory_stream = ROMemoryStream_Create(buffer, buffer_size);

						if (memory_stream != NULL)
						{
							decoder = (Decoder_PxToneNoise*)malloc(sizeof(Decoder_PxToneNoise));

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
			}
		}

		delete pxtn;
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
	short *buffer = (short*)buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		frames_done += ROMemoryStream_Read(decoder->memory_stream, &buffer[frames_done * CHANNEL_COUNT], sizeof(short) * CHANNEL_COUNT, frames_to_do - frames_done);

		if (frames_done != frames_to_do && decoder->loop)
			Decoder_PxToneNoise_Rewind(decoder);
		else
			break;
	}

	return frames_done;
}
