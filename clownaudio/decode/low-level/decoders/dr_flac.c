#include "dr_flac.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include "libs/dr_flac.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "../../common.h"

struct Decoder_DR_FLAC
{
	drflac *backend;
	bool loop;
};

Decoder_DR_FLAC* Decoder_DR_FLAC_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	Decoder_DR_FLAC *decoder = NULL;

	if (data != NULL)
	{
		drflac *backend = drflac_open_memory(data, data_size);

		if (backend != NULL)
		{
			decoder = malloc(sizeof(Decoder_DR_FLAC));

			if (decoder != NULL)
			{
				decoder->backend = backend;
				decoder->loop = loop;

				info->sample_rate = decoder->backend->sampleRate;
				info->channel_count = decoder->backend->channels;
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

	unsigned long frames_done = 0;

	for (;;)
	{
		frames_done += (unsigned long)drflac_read_pcm_frames_s32(decoder->backend, frames_to_do - frames_done, &buffer[frames_done * decoder->backend->channels]);

		if (frames_done != frames_to_do && decoder->loop)
			Decoder_DR_FLAC_Rewind(decoder);
		else
			break;
	}

	return frames_done;
}
