#include "resampled_decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../miniaudio.h"

#include "predecoder.h"

struct ResampledDecoder
{
	Predecoder *predecoder;
	ma_pcm_converter converter;
};

static ma_uint32 PCMConverterCallback(ma_pcm_converter *converter, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)converter;

	Predecoder *predecoder = (Predecoder*)user_data;

	return Predecoder_GetSamples(predecoder, output_buffer, frames_to_do);
}

ResampledDecoderData* ResampledDecoder_LoadData(const unsigned char *file_buffer, size_t file_size)
{
	return (ResampledDecoderData*)Predecoder_DecodeData(file_buffer, file_size);
}

void ResampledDecoder_UnloadData(ResampledDecoderData *data)
{
	Predecoder_UnloadData((PredecoderData*)data);
}

ResampledDecoder* ResampledDecoder_Create(ResampledDecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count)
{
	DecoderInfo info;
	Predecoder *predecoder = Predecoder_Create((PredecoderData*)data, loop, &info);

	if (predecoder != NULL)
	{
		ResampledDecoder *resampled_decoder = malloc(sizeof(ResampledDecoder));

		if (resampled_decoder != NULL)
		{
			resampled_decoder->predecoder = predecoder;

			ma_format format;
			if (info.format == DECODER_FORMAT_S16)
				format = ma_format_s16;
			else if (info.format == DECODER_FORMAT_S32)
				format = ma_format_s32;
			else //if (info.format == DECODER_FORMAT_F32)
				format = ma_format_f32;

			const ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info.channel_count, info.sample_rate, ma_format_f32, channel_count, sample_rate, PCMConverterCallback, resampled_decoder->predecoder);
			ma_pcm_converter_init(&config, &resampled_decoder->converter);

			return resampled_decoder;
		}

		Predecoder_Destroy(predecoder);
	}

	return NULL;
}

void ResampledDecoder_Destroy(ResampledDecoder *resampled_decoder)
{
	if (resampled_decoder != NULL)
	{
		Predecoder_Destroy(resampled_decoder->predecoder);
		free(resampled_decoder);
	}
}

void ResampledDecoder_Rewind(ResampledDecoder *resampled_decoder)
{
	Predecoder_Rewind(resampled_decoder->predecoder);
}

unsigned long ResampledDecoder_GetSamples(ResampledDecoder *resampled_decoder, void *buffer, unsigned long frames_to_do)
{
	return (unsigned long)ma_pcm_converter_read(&resampled_decoder->converter, buffer, frames_to_do);
}
