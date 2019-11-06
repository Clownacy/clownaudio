#include "snes_spc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "libs/snes_spc-0.9.0/snes_spc/spc.h"

#include "common.h"

struct Decoder_SNES_SPC
{
	DecoderData *data;
	SNES_SPC *snes_spc;
//	SPC_Filter *filter;
};

Decoder_SNES_SPC* Decoder_SNES_SPC_Create(DecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	(void)loop;	// Unusable, sadly
	(void)sample_rate;
	(void)channel_count;

	Decoder_SNES_SPC *decoder = NULL;

	if (data != NULL)
	{
		SNES_SPC *snes_spc = spc_new();

		//SPC_Filter *filter = spc_filter_new();

		if (!spc_load_spc(snes_spc, data->file_buffer, data->file_size))
		{
			spc_clear_echo(snes_spc);

			//spc_filter_clear(filter);

			decoder = malloc(sizeof(Decoder_SNES_SPC));

			if (decoder != NULL)
			{
				decoder->data = data;
				decoder->snes_spc = snes_spc;
				//decoder->filter = filter;

				info->sample_rate = spc_sample_rate;
				info->channel_count = 2;
				info->decoded_size = 0;	// Not sure how to get the size of the decoded file yet
				info->format = DECODER_FORMAT_S16;
			}
			else
			{
				spc_delete(snes_spc);
			}
		}
	}

	return decoder;
}

void Decoder_SNES_SPC_Destroy(Decoder_SNES_SPC *decoder)
{
	if (decoder != NULL)
	{
		spc_delete(decoder->snes_spc);
		free(decoder);
	}
}

void Decoder_SNES_SPC_Rewind(Decoder_SNES_SPC *decoder)
{
	spc_delete(decoder->snes_spc);
	spc_load_spc(decoder->snes_spc, decoder->data->file_buffer, decoder->data->file_size);
}

unsigned long Decoder_SNES_SPC_GetSamples(Decoder_SNES_SPC *decoder, void *buffer, unsigned long frames_to_do)
{
	spc_play(decoder->snes_spc, frames_to_do * 2, buffer);

//	spc_filter_run(decoder->filter, buffer, bytes_to_do / sizeof(short));

	return frames_to_do;
}
