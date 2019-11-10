#include "stb_vorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-value"
#endif
#include "libs/stb_vorbis.c"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "common.h"

struct Decoder_STB_Vorbis
{
	DecoderData *data;
	stb_vorbis *instance;
	bool loop;
};

Decoder_STB_Vorbis* Decoder_STB_Vorbis_Create(DecoderData *data, bool loop, DecoderInfo *info)
{
	Decoder_STB_Vorbis *decoder = NULL;

	if (data != NULL)
	{
		stb_vorbis *instance = stb_vorbis_open_memory(data->file_buffer, data->file_size, NULL, NULL);

		if (instance != NULL)
		{
			decoder = malloc(sizeof(Decoder_STB_Vorbis));

			if (decoder != NULL)
			{
				const stb_vorbis_info vorbis_info = stb_vorbis_get_info(instance);

				decoder->instance = instance;
				decoder->data = data;
				decoder->loop = loop;

				info->sample_rate = vorbis_info.sample_rate;
				info->channel_count = vorbis_info.channels;
				info->format = DECODER_FORMAT_F32;
			}
			else
			{
				stb_vorbis_close(instance);
			}
		}
	}

	return decoder;
}

void Decoder_STB_Vorbis_Destroy(Decoder_STB_Vorbis *decoder)
{
	if (decoder != NULL)
	{
		stb_vorbis_close(decoder->instance);
		free(decoder);
	}
}

void Decoder_STB_Vorbis_Rewind(Decoder_STB_Vorbis *decoder)
{
	stb_vorbis_seek_start(decoder->instance);
}

unsigned long Decoder_STB_Vorbis_GetSamples(Decoder_STB_Vorbis *decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		unsigned long frames = stb_vorbis_get_samples_float_interleaved(decoder->instance, decoder->instance->channels, &buffer[frames_done * decoder->instance->channels], (frames_to_do - frames_done) * decoder->instance->channels);

		if (frames == 0)
		{
			if (decoder->loop)
				Decoder_STB_Vorbis_Rewind(decoder);
			else
				break;
		}

		frames_done += frames;

		if (frames == frames_to_do)
			break;
	}

	return frames_done;
}
