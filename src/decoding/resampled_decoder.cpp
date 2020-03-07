/*
 *  (C) 2019 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include "resampled_decoder.h"

#include <stddef.h>
#include <stdlib.h>

#include "../miniaudio.h"

#include "decoder_selector.h"

#define RESAMPLE_BUFFER_SIZE 0x1000
#define CHANNEL_COUNT 2

struct ResampledDecoder
{
	DecoderSelector *decoder;
	ma_data_converter converter;
	unsigned long sample_rate;
	size_t size_of_frame;
	unsigned char buffer[RESAMPLE_BUFFER_SIZE];
	size_t buffer_remaining;
};

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
		ResampledDecoder *resampled_decoder = (ResampledDecoder*)malloc(sizeof(ResampledDecoder));

		if (resampled_decoder != NULL)
		{
			resampled_decoder->decoder = (DecoderSelector*)decoder;

			ma_format format;
			if (info.format == DECODER_FORMAT_S16)
				format = ma_format_s16;
			else if (info.format == DECODER_FORMAT_S32)
				format = ma_format_s32;
			else //if (info.format == DECODER_FORMAT_F32)
				format = ma_format_f32;

			ma_data_converter_config config = ma_data_converter_config_init(format, ma_format_f32, info.channel_count, CHANNEL_COUNT, info.sample_rate, sample_rate);
			config.resampling.allowDynamicSampleRate = MA_TRUE;

			if (ma_data_converter_init(&config, &resampled_decoder->converter) == MA_SUCCESS)
			{
				resampled_decoder->size_of_frame = ma_get_bytes_per_sample(format) * CHANNEL_COUNT;
				resampled_decoder->buffer_remaining = 0;
				resampled_decoder->sample_rate = sample_rate;

				return resampled_decoder;
			}

			free(resampled_decoder);
		}

		DecoderSelector_Destroy((DecoderSelector*)decoder);
	}

	return NULL;
}

void ResampledDecoder_Destroy(ResampledDecoder *resampled_decoder)
{
	ma_data_converter_uninit(&resampled_decoder->converter);
	DecoderSelector_Destroy(resampled_decoder->decoder);
	free(resampled_decoder);
}

void ResampledDecoder_Rewind(ResampledDecoder *resampled_decoder)
{
	DecoderSelector_Rewind(resampled_decoder->decoder);
}

size_t ResampledDecoder_GetSamples(ResampledDecoder *resampled_decoder, void *buffer_void, size_t frames_to_do)
{
	float (*buffer)[CHANNEL_COUNT] = (float(*)[CHANNEL_COUNT])buffer_void;

	size_t frames_done = 0;

	while (frames_done != frames_to_do)
	{
		if (resampled_decoder->buffer_remaining == 0)
		{
			resampled_decoder->buffer_remaining = DecoderSelector_GetSamples(resampled_decoder->decoder, resampled_decoder->buffer, RESAMPLE_BUFFER_SIZE / resampled_decoder->size_of_frame);

			if (resampled_decoder->buffer_remaining == 0)
				return frames_done;	// Sample end
		}

		ma_uint64 frames_in = resampled_decoder->buffer_remaining;
		ma_uint64 frames_out = frames_to_do - frames_done;
		ma_data_converter_process_pcm_frames(&resampled_decoder->converter, &resampled_decoder->buffer[RESAMPLE_BUFFER_SIZE - (resampled_decoder->buffer_remaining * resampled_decoder->size_of_frame)], &frames_in, &buffer[frames_done], &frames_out);

		resampled_decoder->buffer_remaining -= frames_in;
		frames_done += frames_out;
	}

	return frames_to_do;
}

void ResampledDecoder_SetLoop(ResampledDecoder *resampled_decoder, bool loop)
{
	DecoderSelector_SetLoop(resampled_decoder->decoder, loop);
}

void ResampledDecoder_SetSampleRate(ResampledDecoder *resampled_decoder, unsigned long sample_rate)
{
	ma_data_converter_set_rate(&resampled_decoder->converter, sample_rate, resampled_decoder->sample_rate);
}
