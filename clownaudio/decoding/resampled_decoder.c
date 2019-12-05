#include "resampled_decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../miniaudio.h"

#include "decoder_selector.h"
#include "predecoder.h"

struct ResampledDecoder
{
	DecoderSelector *decoder;
	ma_pcm_converter converter;
};

static ma_uint32 PCMConverterCallback(ma_pcm_converter *converter, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)converter;

	ResampledDecoder *resampled_decoder = (ResampledDecoder*)user_data;

	return DecoderSelector_GetSamples(resampled_decoder->decoder, output_buffer, frames_to_do);
}

ResampledDecoderData* ResampledDecoder_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode)
{
	return DecoderSelector_LoadData(file_buffer, file_size, predecode);
}

void ResampledDecoder_UnloadData(ResampledDecoderData *data)
{
	DecoderSelector_UnloadData(data);
}

ResampledDecoder* ResampledDecoder_Create(ResampledDecoderData *data, bool loop, unsigned long sample_rate)
{
	DecoderInfo info;
	void *decoder = DecoderSelector_Create(data, loop, &info);

	if (decoder != NULL)
	{
		ResampledDecoder *resampled_decoder = malloc(sizeof(ResampledDecoder));

		if (resampled_decoder != NULL)
		{
			resampled_decoder->decoder = decoder;

			ma_format format;
			if (info.format == DECODER_FORMAT_S16)
				format = ma_format_s16;
			else if (info.format == DECODER_FORMAT_S32)
				format = ma_format_s32;
			else //if (info.format == DECODER_FORMAT_F32)
				format = ma_format_f32;

			ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info.channel_count, info.sample_rate, ma_format_f32, 2, sample_rate, PCMConverterCallback, resampled_decoder);
			config.allowDynamicSampleRate = MA_TRUE;

			if (ma_pcm_converter_init(&config, &resampled_decoder->converter) == MA_SUCCESS)
				return resampled_decoder;

			free(resampled_decoder);
		}

		DecoderSelector_Destroy(decoder);
	}

	return NULL;
}

void ResampledDecoder_Destroy(ResampledDecoder *resampled_decoder)
{
	DecoderSelector_Destroy(resampled_decoder->decoder);
	free(resampled_decoder);
}

void ResampledDecoder_Rewind(ResampledDecoder *resampled_decoder)
{
	DecoderSelector_Rewind(resampled_decoder->decoder);
}

size_t ResampledDecoder_GetSamples(ResampledDecoder *resampled_decoder, void *buffer, size_t frames_to_do)
{
	return ma_pcm_converter_read(&resampled_decoder->converter, buffer, frames_to_do);
}

void ResampledDecoder_SetSampleRate(ResampledDecoder *resampled_decoder, unsigned long sample_rate)
{
	if (resampled_decoder != NULL)
		ma_pcm_converter_set_input_sample_rate(&resampled_decoder->converter, sample_rate);
}