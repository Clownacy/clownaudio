#include "predecoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoder.h"
#include "memory_stream.h"

struct PredecoderData
{
	void *decoded_data;
	size_t decoded_data_size;
	size_t channel_count;	// TODO - Get rid of this when we make stereo the only option
};

struct Predecoder
{
	PredecoderData *data;	// TODO - And this
	ROMemoryStream *stream;
	bool loop;
};

PredecoderData* Predecoder_DecodeData(const unsigned char *file_buffer, size_t file_size, unsigned long sample_rate, unsigned int channel_count)
{
	PredecoderData *predecoder_data = NULL;

	DecoderData *decoder_data = Decoder_LoadData(file_buffer, file_size);

	if (decoder_data != NULL)
	{
		Decoder *decoder = Decoder_Create(decoder_data, false, sample_rate, channel_count);

		if (decoder != NULL)
		{
			MemoryStream *stream = MemoryStream_Create(false);

			if (stream != NULL)
			{
				predecoder_data = malloc(sizeof(PredecoderData));

				if (predecoder_data != NULL)
				{
					for (;;)
					{
						float buffer[0x1000];

						unsigned long samples_read = Decoder_GetSamples(decoder, buffer, 0x1000 / channel_count) * channel_count;

						MemoryStream_Write(stream, buffer, sizeof(float), samples_read);

						if (samples_read != 0x1000)
							break;
					}

					predecoder_data->decoded_data = MemoryStream_GetBuffer(stream);
					predecoder_data->decoded_data_size = MemoryStream_GetPosition(stream);
					predecoder_data->channel_count = channel_count;
				}

				MemoryStream_Destroy(stream);
			}
			
			Decoder_Destroy(decoder);
		}

		Decoder_UnloadData(decoder_data);
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

Predecoder* Predecoder_Create(PredecoderData *data, bool loop)
{
	Predecoder *predecoder = NULL;

	if (data != NULL)
	{
		predecoder = malloc(sizeof(Predecoder));

		if (predecoder != NULL)
		{
			predecoder->data = data;
			predecoder->stream = ROMemoryStream_Create(data->decoded_data, data->decoded_data_size);
			predecoder->loop = loop;

			if (predecoder->stream == NULL)
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
	ROMemoryStream_Destroy(predecoder->stream);
}

void Predecoder_Rewind(Predecoder *predecoder)
{
	ROMemoryStream_Rewind(predecoder->stream);
}

unsigned long Predecoder_GetSamples(Predecoder *predecoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = ROMemoryStream_Read(predecoder->stream, buffer + (frames_done_total * predecoder->data->channel_count), predecoder->data->channel_count * sizeof(float), frames_to_do - frames_done_total);

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (predecoder->loop)
				Predecoder_Rewind(predecoder);
			else
				break;
		}
	}

	return frames_done_total;
}
