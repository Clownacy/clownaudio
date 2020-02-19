#include "predecoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "../miniaudio.h"

#include "decoders/memory_stream.h"

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

typedef struct DecoderMetadata
{
	Decoder *decoder;
	size_t (*GetSamples)(Decoder *decoder, void *buffer, size_t frames_to_do);
} DecoderMetadata;

static ma_uint32 PCMConverterCallback(ma_pcm_converter *converter, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)converter;

	DecoderMetadata *decoder_metadata = user_data;

	return decoder_metadata->GetSamples(decoder_metadata->decoder, output_buffer, frames_to_do);
}

PredecoderData* Predecoder_DecodeData(DecoderInfo *info, Decoder *decoder, size_t (*decoder_get_samples_function)(Decoder *decoder, void *buffer, size_t frames_to_do))
{
	PredecoderData *predecoder_data = NULL;

	MemoryStream *memory_stream = MemoryStream_Create(false);

	if (memory_stream != NULL)
	{
		ma_format format;
		if (info->format == DECODER_FORMAT_S16)
			format = ma_format_s16;
		else if (info->format == DECODER_FORMAT_S32)
			format = ma_format_s32;
		else //if (info->format == DECODER_FORMAT_F32)
			format = ma_format_f32;

		DecoderMetadata decoder_metadata;
		decoder_metadata.decoder = decoder;
		decoder_metadata.GetSamples = decoder_get_samples_function;

		const ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info->channel_count, info->sample_rate, ma_format_f32, 2, info->sample_rate, PCMConverterCallback, &decoder_metadata);

		ma_pcm_converter converter;
		if (ma_pcm_converter_init(&config, &converter) == MA_SUCCESS)
		{
			predecoder_data = malloc(sizeof(PredecoderData));

			if (predecoder_data != NULL)
			{
				for (;;)
				{
					float buffer[0x1000];

					size_t samples_read = ma_pcm_converter_read(&converter, buffer, 0x1000 / CHANNEL_COUNT) * CHANNEL_COUNT;

					MemoryStream_Write(memory_stream, buffer, sizeof(float), samples_read);

					if (samples_read != 0x1000)
						break;
				}

				predecoder_data->decoded_data = MemoryStream_GetBuffer(memory_stream);
				predecoder_data->decoded_data_size = MemoryStream_GetPosition(memory_stream);
				predecoder_data->sample_rate = info->sample_rate;
			}
		}

		MemoryStream_Destroy(memory_stream);
	}

	return predecoder_data;
}

void Predecoder_UnloadData(PredecoderData *data)
{
	free(data->decoded_data);
	free(data);
}

Predecoder* Predecoder_Create(PredecoderData *data, bool loop, DecoderInfo *info)
{
	Predecoder *predecoder = malloc(sizeof(Predecoder));

	if (predecoder != NULL)
	{
		predecoder->memory_stream = ROMemoryStream_Create(data->decoded_data, data->decoded_data_size);

		if (predecoder->memory_stream != NULL)
		{
			predecoder->loop = loop;

			info->sample_rate = data->sample_rate;
			info->channel_count = CHANNEL_COUNT;
			info->format = DECODER_FORMAT_F32;

			return predecoder;
		}

		free(predecoder);
	}

	return NULL;
}

void Predecoder_Destroy(Predecoder *predecoder)
{
	ROMemoryStream_Destroy(predecoder->memory_stream);
}

void Predecoder_Rewind(Predecoder *predecoder)
{
	ROMemoryStream_Rewind(predecoder->memory_stream);
}

size_t Predecoder_GetSamples(Predecoder *predecoder, void *buffer_void, size_t frames_to_do)
{
	float *buffer = buffer_void;

	size_t frames_done = 0;

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

void Predecoder_SetLoop(Predecoder *predecoder, bool loop)
{
	predecoder->loop = loop;
}
