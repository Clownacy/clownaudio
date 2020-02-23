#include "libxmp-lite.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define BUILDING_STATIC
#include <xmp.h>

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct Decoder
{
	xmp_context context;
	bool loop;
};

Decoder* Decoder_libXMPLite_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	Decoder *decoder = NULL;

	xmp_context context = xmp_create_context();

	if (!xmp_load_module_from_memory(context, (void*)data, data_size))
	{
		xmp_start_player(context, SAMPLE_RATE, 0);

		decoder = malloc(sizeof(Decoder));

		if (decoder != NULL)
		{
			decoder->context = context;
			decoder->loop = loop;

			info->sample_rate = SAMPLE_RATE;
			info->channel_count = CHANNEL_COUNT;
			info->format = DECODER_FORMAT_S16;
			info->is_complex = true;

			return decoder;
		}

		xmp_end_player(context);
		xmp_release_module(context);
	}

	xmp_free_context(context);

	return NULL;
}

void Decoder_libXMPLite_Destroy(Decoder *decoder)
{
	xmp_end_player(decoder->context);
	xmp_release_module(decoder->context);
	xmp_free_context(decoder->context);
	free(decoder);
}

void Decoder_libXMPLite_Rewind(Decoder *decoder)
{
	xmp_seek_time(decoder->context, 0);
}

size_t Decoder_libXMPLite_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do)
{
	xmp_play_buffer(decoder->context, buffer, frames_to_do * CHANNEL_COUNT * sizeof(short), !decoder->loop);

	return frames_to_do;
}
