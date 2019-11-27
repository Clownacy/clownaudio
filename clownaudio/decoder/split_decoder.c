#include "split_decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "resampled_decoder.h"

#define CHANNEL_COUNT 2

struct SplitDecoderData
{
	ResampledDecoderData *resampled_decoder_data[2];
};

struct SplitDecoder
{
	ResampledDecoder *resampled_decoder[2];
	unsigned int current_half;
};

SplitDecoderData* SplitDecoder_LoadData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2)
{
	SplitDecoderData *data = malloc(sizeof(SplitDecoderData));

	if (data != NULL)
	{
		data->resampled_decoder_data[0] = ResampledDecoder_LoadData(file_buffer1, file_size1);
		data->resampled_decoder_data[1] = ResampledDecoder_LoadData(file_buffer2, file_size2);

		if (data->resampled_decoder_data[0] != NULL || data->resampled_decoder_data[1] != NULL)
			return data;

		free(data);
	}

	return NULL;
}

void SplitDecoder_UnloadData(SplitDecoderData *data)
{
	if (data != NULL)
	{
		ResampledDecoder_UnloadData(data->resampled_decoder_data[0]);
		ResampledDecoder_UnloadData(data->resampled_decoder_data[1]);
		free(data);
	}
}

SplitDecoder* SplitDecoder_Create(SplitDecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count)
{
	SplitDecoder *split_decoder = NULL;

	if (data != NULL)
	{
		split_decoder = malloc(sizeof(SplitDecoder));

		if (split_decoder != NULL)
		{
			if (data->resampled_decoder_data[0] != NULL && data->resampled_decoder_data[1] != NULL)
			{
				split_decoder->resampled_decoder[0] = ResampledDecoder_Create(data->resampled_decoder_data[0], false, sample_rate, channel_count);
				split_decoder->resampled_decoder[1] = ResampledDecoder_Create(data->resampled_decoder_data[1], loop, sample_rate, channel_count);
				split_decoder->current_half = 0;
			}
			else if (data->resampled_decoder_data[0] != NULL)
			{
				split_decoder->resampled_decoder[1] = ResampledDecoder_Create(data->resampled_decoder_data[0], loop, sample_rate, channel_count);
				split_decoder->current_half = 1;
			}
			else if (data->resampled_decoder_data[1] != NULL)
			{
				split_decoder->resampled_decoder[1] = ResampledDecoder_Create(data->resampled_decoder_data[1], loop, sample_rate, channel_count);
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
		ResampledDecoder_Destroy(split_decoder->resampled_decoder[0]);
		ResampledDecoder_Destroy(split_decoder->resampled_decoder[1]);
		free(split_decoder);
	}
}

void SplitDecoder_Rewind(SplitDecoder *split_decoder)
{
	if (split_decoder != NULL)
	{
		ResampledDecoder_Rewind(split_decoder->resampled_decoder[0]);
		ResampledDecoder_Rewind(split_decoder->resampled_decoder[1]);
	}
}

unsigned long SplitDecoder_GetSamples(SplitDecoder *split_decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		frames_done += ResampledDecoder_GetSamples(split_decoder->resampled_decoder[split_decoder->current_half], &buffer[frames_done * CHANNEL_COUNT], frames_to_do - frames_done);

		if (frames_done != frames_to_do && split_decoder->current_half == 0)
			split_decoder->current_half = 1;
		else
			break;
	}

	return frames_done;
}
