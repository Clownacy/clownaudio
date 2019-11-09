#include "pxtone.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "libs/pxtone/pxtnService.h"
#include "libs/pxtone/pxtnError.h"

#include "common.h"

struct Decoder_PxTone
{
	DecoderData *data;
	pxtnService *pxtn;
	bool loop;
};

Decoder_PxTone* Decoder_PxTone_Create(DecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	if (data != NULL)
	{
		pxtnService *pxtn = new pxtnService();

		if (pxtn->init() == pxtnOK)
		{
			if (pxtn->set_destination_quality(channel_count, sample_rate))
			{
				pxtnDescriptor desc;

				if (desc.set_memory_r((void*)data->file_buffer, data->file_size) && pxtn->read(&desc) == pxtnOK && pxtn->tones_ready() == pxtnOK)
				{
					pxtnVOMITPREPARATION prep = pxtnVOMITPREPARATION();
					if (loop)
						prep.flags |= pxtnVOMITPREPFLAG_loop;
					prep.start_pos_float = 0;
					prep.master_volume = 0.80f;

					if (pxtn->moo_preparation(&prep))
					{
						Decoder_PxTone *decoder = (Decoder_PxTone*)malloc(sizeof(Decoder_PxTone));

						if (decoder != NULL)
						{
							decoder->pxtn = pxtn;
							decoder->data = data;
							decoder->loop = loop;

							info->sample_rate = sample_rate;
							info->channel_count = channel_count;
							info->format = DECODER_FORMAT_S16;

							return decoder;
						}
					}
				}

				pxtn->evels->Release();
			}
		}

		delete pxtn;
	}

	return NULL;
}

void Decoder_PxTone_Destroy(Decoder_PxTone *decoder)
{
	if (decoder != NULL)
	{
		delete decoder->pxtn;
		free(decoder);
	}
}

void Decoder_PxTone_Rewind(Decoder_PxTone *decoder)
{
	pxtnVOMITPREPARATION prep = pxtnVOMITPREPARATION();
	if (decoder->loop)
		prep.flags |= pxtnVOMITPREPFLAG_loop;
	prep.start_pos_float = 0;
	prep.master_volume = 0.80f;

	decoder->pxtn->moo_preparation(&prep);
}

unsigned long Decoder_PxTone_GetSamples(Decoder_PxTone *decoder, void *buffer, unsigned long frames_to_do)
{
	const size_t size_of_frame = sizeof(short) * 2;

	memset(buffer, 0, frames_to_do * size_of_frame);

	return decoder->pxtn->Moo(buffer, frames_to_do * size_of_frame);
}
