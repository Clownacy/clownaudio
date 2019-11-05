#include "dr_flac.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG
#include "libs/dr_flac.h"

#include "common.h"

struct Decoder_DR_FLAC
{
	DecoderData *data;
	drflac *backend;
	bool loop;
};

Decoder_DR_FLAC* Decoder_DR_FLAC_Create(DecoderData *data, bool loop, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	(void)sample_rate;
	(void)channel_count;

	Decoder_DR_FLAC *decoder = NULL;

	if (data != NULL)
	{
		drflac *backend = drflac_open_memory(data->file_buffer, data->file_size);

		if (backend != NULL)
		{
			decoder = malloc(sizeof(Decoder_DR_FLAC));

			if (decoder != NULL)
			{
				decoder->backend = backend;
				decoder->data = data;
				decoder->loop = loop;

				info->sample_rate = decoder->backend->sampleRate;
				info->channel_count = decoder->backend->channels;
				info->decoded_size = (unsigned long)decoder->backend->totalSampleCount * sizeof(drflac_int32);
				info->format = DECODER_FORMAT_S32;
			}
			else
			{
				drflac_close(backend);
			}
		}
	}

	return decoder;
}

void Decoder_DR_FLAC_Destroy(Decoder_DR_FLAC *decoder)
{
	if (decoder != NULL)
	{
		drflac_close(decoder->backend);
		free(decoder);
	}
}

void Decoder_DR_FLAC_Rewind(Decoder_DR_FLAC *decoder)
{
	drflac_seek_to_pcm_frame(decoder->backend, 0);
}

unsigned long Decoder_DR_FLAC_GetSamples(Decoder_DR_FLAC *decoder, void *buffer_void, unsigned long frames_to_do)
{
	drflac_int32 *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = (unsigned long)drflac_read_pcm_frames_s32(decoder->backend, frames_to_do - frames_done_total, buffer + (frames_done_total * decoder->backend->channels));

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (decoder->loop)
				Decoder_DR_FLAC_Rewind(decoder);
			else
				break;
		}
	}

	return frames_done_total;
}
