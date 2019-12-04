#include "predecoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../miniaudio.h"

#include "memory_stream.h"
#include "low-level/decoder_selector.h"

#define CHANNEL_COUNT 2

struct PredecoderData
{
	void *decoded_data;
	size_t decoded_data_size;
	unsigned long sample_rate;
};

struct Predecoder
{
	ROMemoryStream *memory_stream;
	bool loop;
};

static ma_uint32 PCMConverterCallback(ma_pcm_converter *converter, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)converter;

	LowLevelDecoderSelector *decoder = user_data;

	return LowLevelDecoderSelector_GetSamples(decoder, output_buffer, frames_to_do);
}

PredecoderData* Predecoder_DecodeData(const unsigned char *data, size_t data_size)
{
	PredecoderData *predecoder_data = NULL;

	DecoderInfo info;
	LowLevelDecoderSelector *decoder = LowLevelDecoderSelector_Create(data, data_size, false, &info);

	if (decoder != NULL)
	{
		MemoryStream *memory_stream = MemoryStream_Create(false);

		if (memory_stream != NULL)
		{
			ma_format format;
			if (info.format == DECODER_FORMAT_S16)
				format = ma_format_s16;
			else if (info.format == DECODER_FORMAT_S32)
				format = ma_format_s32;
			else //if (info.format == DECODER_FORMAT_F32)
				format = ma_format_f32;

			const ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info.channel_count, info.sample_rate, ma_format_f32, 2, info.sample_rate, PCMConverterCallback, decoder);

			ma_pcm_converter converter;
			if (ma_pcm_converter_init(&config, &converter) == MA_SUCCESS)
			{
				predecoder_data = malloc(sizeof(PredecoderData));

				if (predecoder_data != NULL)
				{
					for (;;)
					{
						float buffer[0x1000];

						unsigned long samples_read = (unsigned long)ma_pcm_converter_read(&converter, buffer, 0x1000 / CHANNEL_COUNT) * CHANNEL_COUNT;

						MemoryStream_Write(memory_stream, buffer, sizeof(float), samples_read);

						if (samples_read != 0x1000)
							break;
					}

					predecoder_data->decoded_data = MemoryStream_GetBuffer(memory_stream);
					predecoder_data->decoded_data_size = MemoryStream_GetPosition(memory_stream);
					predecoder_data->sample_rate = info.sample_rate;
				}
			}

			MemoryStream_Destroy(memory_stream);
		}

		LowLevelDecoderSelector_Destroy(decoder);
	}

	return predecoder_data;
}

void Predecoder_UnloadData(PredecoderData *data)
{
	if (data != NULL)
	{
		free(data->decoded_data);
		free(data);
	}
}

Predecoder* Predecoder_Create(PredecoderData *data, bool loop, DecoderInfo *info)
{
	Predecoder *predecoder = NULL;

	if (data != NULL)
	{
		predecoder = malloc(sizeof(Predecoder));

		if (predecoder != NULL)
		{
			predecoder->memory_stream = ROMemoryStream_Create(data->decoded_data, data->decoded_data_size);
			predecoder->loop = loop;

			info->sample_rate = data->sample_rate;
			info->channel_count = CHANNEL_COUNT;
			info->format = DECODER_FORMAT_F32;

			if (predecoder->memory_stream == NULL)
			{
				free(predecoder);
				predecoder = NULL;
			}
		}
	}

	return predecoder;
}

void Predecoder_Destroy(Predecoder *predecoder)
{
	ROMemoryStream_Destroy(predecoder->memory_stream);
}

void Predecoder_Rewind(Predecoder *predecoder)
{
	ROMemoryStream_Rewind(predecoder->memory_stream);
}

unsigned long Predecoder_GetSamples(Predecoder *predecoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		frames_done += ROMemoryStream_Read(predecoder->memory_stream, &buffer[frames_done * CHANNEL_COUNT], sizeof(float) * CHANNEL_COUNT, frames_to_do - frames_done);

		if (frames_done != frames_to_do && predecoder->loop)
			Predecoder_Rewind(predecoder);
		else
			break;
	}

	return frames_done;
}
