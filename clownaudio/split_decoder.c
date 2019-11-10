#include "split_decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "predecoder.h"

struct SplitDecoderData
{
	PredecoderData *predecoder_data[2];
	unsigned int size_of_frame;	// TODO - Get rid of this when we force stereo
};

struct SplitDecoder
{
	SplitDecoderData *data;
	Predecoder *predecoder[2];
	unsigned int current_half;
};

SplitDecoderData* SplitDecoder_DecodeData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, unsigned long sample_rate, unsigned int channel_count)
{
	SplitDecoderData *data = malloc(sizeof(SplitDecoderData));

	if (data != NULL)
	{
		data->predecoder_data[0] = Predecoder_DecodeData(file_buffer1, file_size1, sample_rate, channel_count);
		data->predecoder_data[1] = Predecoder_DecodeData(file_buffer2, file_size2, sample_rate, channel_count);
		data->size_of_frame = channel_count * sizeof(float);

		if (data->predecoder_data[0] != NULL || data->predecoder_data[1] != NULL)
			return data;

		free(data);
	}

	return NULL;
}

void SplitDecoder_UnloadData(SplitDecoderData *data)
{
	if (data != NULL)
	{
		Predecoder_UnloadData(data->predecoder_data[0]);
		Predecoder_UnloadData(data->predecoder_data[1]);
		free(data);
	}
}

SplitDecoder* SplitDecoder_Create(SplitDecoderData *data, bool loop)
{
	SplitDecoder *split_decoder = NULL;

	if (data != NULL)
	{
		split_decoder = malloc(sizeof(SplitDecoder));

		if (split_decoder != NULL)
		{
			split_decoder->data = data;

			if (data->predecoder_data[0] != NULL && data->predecoder_data[1] != NULL)
			{
				split_decoder->predecoder[0] = Predecoder_Create(data->predecoder_data[0], false);
				split_decoder->predecoder[1] = Predecoder_Create(data->predecoder_data[1], loop);
				split_decoder->current_half = 0;
			}
			else if (data->predecoder_data[0] != NULL)
			{
				split_decoder->predecoder[1] = Predecoder_Create(data->predecoder_data[0], loop);
				split_decoder->current_half = 1;
			}
			else if (data->predecoder_data[1] != NULL)
			{
				split_decoder->predecoder[1] = Predecoder_Create(data->predecoder_data[1], loop);
				split_decoder->current_half = 1;
			}
		}
	}

	return split_decoder;
}

void SplitDecoder_Destroy(SplitDecoder *split_decoder)
{
	if (split_decoder != NULL)
	{
		Predecoder_Destroy(split_decoder->predecoder[0]);
		Predecoder_Destroy(split_decoder->predecoder[1]);
		free(split_decoder);
	}
}

void SplitDecoder_Rewind(SplitDecoder *split_decoder)
{
	if (split_decoder != NULL)
	{
		Predecoder_Rewind(split_decoder->predecoder[0]);
		Predecoder_Rewind(split_decoder->predecoder[1]);
	}
}

unsigned long SplitDecoder_GetSamples(SplitDecoder *split_decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		frames_done += Predecoder_GetSamples(split_decoder->predecoder[split_decoder->current_half], &buffer[frames_done * split_decoder->data->size_of_frame], frames_to_do - frames_done);

		if (frames_done == frames_to_do)
		{
			break;
		}
		else
		{
			if (split_decoder->current_half == 0)
				split_decoder->current_half = 1;
			else
				break;
		}
	}

	return frames_done;
}
