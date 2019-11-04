#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "miniaudio.h"

#include "decoders/common.h"
#include "decoders/dr_flac.h"

struct Decoder
{
	Decoder_DR_FLAC *backend;
	ma_pcm_converter converter;
};

static ma_uint32 PCMConverterCallback(ma_pcm_converter *converter, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)converter;

	return Decoder_DR_FLAC_GetSamples((Decoder_DR_FLAC*)user_data, output_buffer, frames_to_do);
}

DecoderData* Decoder_LoadData(const unsigned char *file_buffer, size_t file_size)
{
	DecoderData *data = NULL;

	if (file_buffer != NULL)
	{
		data = malloc(sizeof(DecoderData));

		if (data != NULL)
		{
			data->file_buffer = file_buffer;
			data->file_size = file_size;
		}
	}

	return data;
}

void Decoder_UnloadData(DecoderData *data)
{
	//if (data != NULL)
	//{
	//	free(data->file_buffer);
		free(data);
	//}
}

Decoder* Decoder_Create(DecoderData *data, bool loop, unsigned int sample_rate, unsigned int channel_count)
{
	Decoder *decoder = malloc(sizeof(Decoder));

	if (decoder != NULL)
	{
		DecoderInfo info;
		decoder->backend = Decoder_DR_FLAC_Create(data, loop, &info);	// TODO: Format-negotiation

		if (decoder->backend != NULL)
		{
			ma_format format;
			switch (info.format)
			{
				case DECODER_FORMAT_S16:
					format = ma_format_s16;
					break;

				case DECODER_FORMAT_S32:
					format = ma_format_s32;
					break;

				case DECODER_FORMAT_F32:
					format = ma_format_f32;
					break;
			}

			const ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info.channel_count, info.sample_rate, ma_format_f32, channel_count, sample_rate, PCMConverterCallback, decoder->backend);
			ma_pcm_converter_init(&config, &decoder->converter);
		}
		else
		{
			free(decoder);
			decoder = NULL;
		}
	}

	return decoder;
}

void Decoder_Destroy(Decoder *decoder)
{
	if (decoder != NULL)
	{
		Decoder_DR_FLAC_Destroy(decoder->backend);
		free(decoder);
	}
}

void Decoder_Rewind(Decoder *decoder)
{
	Decoder_DR_FLAC_Rewind(decoder->backend);
}

unsigned long Decoder_GetSamples(Decoder *decoder, void *buffer_void, unsigned long frames_to_do)
{
	return (unsigned long)ma_pcm_converter_read(&decoder->converter, buffer_void, frames_to_do);
}
