#include "pxtone.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

//#include "libs/pxtone/pxtnService.h"
//#include "libs/pxtone/pxtnError.h"
#include "libs/pxtone/shim.h"

#include "common.h"

struct Decoder_PxTone
{
	DecoderData *data;
	pxtnService *pxtn;
	bool loop;
};

Decoder_PxTone* Decoder_PxTone_Create(DecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	Decoder_PxTone *decoder = NULL;

	if (data != NULL)
	{
		pxtnService *pxtn = PxTone_Open(data->file_buffer, data->file_size, loop, sample_rate, channel_count);

		if (pxtn != NULL)
		{
			decoder = (Decoder_PxTone*)malloc(sizeof(Decoder_PxTone));

			if (decoder != NULL)
			{
				decoder->pxtn = pxtn;
				decoder->data = data;
				decoder->loop = loop;

				info->sample_rate = sample_rate;
				info->channel_count = channel_count;
				info->format = DECODER_FORMAT_S16;
			}
			else
			{
				PxTone_Close(pxtn);
			}
		}
	}

	return decoder;
}

void Decoder_PxTone_Destroy(Decoder_PxTone *decoder)
{
	if (decoder != NULL)
	{
		PxTone_Close(decoder->pxtn);
		free(decoder);
	}
}

void Decoder_PxTone_Rewind(Decoder_PxTone *decoder)
{
	PxTone_Rewind(decoder->pxtn, decoder->loop);
}

unsigned long Decoder_PxTone_GetSamples(Decoder_PxTone *decoder, void *buffer, unsigned long frames_to_do)
{
	const size_t size_of_frame = sizeof(short) * 2;

	memset(buffer, 0, frames_to_do * size_of_frame);

	return PxTone_GetSamples(decoder->pxtn, buffer, frames_to_do * size_of_frame);
}
