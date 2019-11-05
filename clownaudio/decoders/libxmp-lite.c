#include "libxmp-lite.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define BUILDING_STATIC
#include <xmp.h>

#include "common.h"

struct Decoder_libXMPLite
{
	DecoderData *data;
	xmp_context context;
	bool loops;
	unsigned int channel_count;
};

Decoder_libXMPLite* Decoder_libXMPLite_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	Decoder_libXMPLite *decoder = NULL;

	if (data != NULL)
	{
		xmp_context context = xmp_create_context();

		if (!xmp_load_module_from_memory(context, (void*)data->file_buffer, data->file_size))
		{
			xmp_start_player(context, sample_rate, 0);

			decoder = malloc(sizeof(Decoder_libXMPLite));

			if (decoder != NULL)
			{
				decoder->context = context;
				decoder->data = data;
				decoder->loops = loops;
				decoder->channel_count = channel_count;

				info->sample_rate = sample_rate;
				info->channel_count = channel_count;
				info->format = DECODER_FORMAT_S16;
			}
			else
			{
				xmp_end_player(decoder->context);
				xmp_release_module(decoder->context);
				xmp_free_context(decoder->context);
			}
		}
	}

	return decoder;
}

void Decoder_libXMPLite_Destroy(Decoder_libXMPLite *decoder)
{
	if (decoder != NULL)
	{
		xmp_end_player(decoder->context);
		xmp_release_module(decoder->context);
		xmp_free_context(decoder->context);
		free(decoder);
	}
}

void Decoder_libXMPLite_Rewind(Decoder_libXMPLite *decoder)
{
	xmp_seek_time(decoder->context, 0);
}

unsigned long Decoder_libXMPLite_GetSamples(Decoder_libXMPLite *decoder, void *buffer, unsigned long frames_to_do)
{
	xmp_play_buffer(decoder->context, buffer, frames_to_do * decoder->channel_count * sizeof(short), !decoder->loops);

	return frames_to_do;
}
