#include "libsndfile.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "common.h"
#include "memory_stream.h"

struct Decoder
{
	ROMemoryStream *memory_stream;
	SNDFILE *sndfile;
};

static sf_count_t fread_wrapper(void *output, sf_count_t count, void *user)
{
	return ROMemoryStream_Read(user, output, 1, count);
}

static sf_count_t fseek_wrapper(sf_count_t offset, int origin, void *user)
{
	enum MemoryStream_Origin memory_stream_origin;
	switch (origin)
	{
		case SEEK_SET:
			memory_stream_origin = MEMORYSTREAM_START;
			break;

		case SEEK_CUR:
			memory_stream_origin = MEMORYSTREAM_CURRENT;
			break;

		case SEEK_END:
			memory_stream_origin = MEMORYSTREAM_END;
			break;

		default:
			return -1;
	}

	return (ROMemoryStream_SetPosition(user, offset, memory_stream_origin) ? 0 : -1);
}

static sf_count_t ftell_wrapper(void *user)
{
	return ROMemoryStream_GetPosition(user);
}

static sf_count_t GetStreamSize(void *user)
{
	const sf_count_t old_offset = ftell_wrapper(user);

	fseek_wrapper(0, SEEK_END, user);
	const sf_count_t size = ftell_wrapper(user);

	fseek_wrapper(old_offset, SEEK_SET, user);

	return size;
}

static SF_VIRTUAL_IO sfvirtual = {
	GetStreamSize,
	fseek_wrapper,
	fread_wrapper,
	NULL,
	ftell_wrapper
};

Decoder* Decoder_libSndfile_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	(void)loop;	// This is ignored in simple decoders

	Decoder *decoder = NULL;

	ROMemoryStream *memory_stream = ROMemoryStream_Create(data, data_size);

	if (memory_stream != NULL)
	{
		SF_INFO sf_info;
		memset(&sf_info, 0, sizeof(SF_INFO));

		SNDFILE *sndfile = sf_open_virtual(&sfvirtual, SFM_READ, &sf_info, memory_stream);

		if (sndfile != NULL)
		{
			decoder = malloc(sizeof(Decoder));

			if (decoder != NULL)
			{
				decoder->sndfile = sndfile;
				decoder->memory_stream = memory_stream;

				info->sample_rate = sf_info.samplerate;
				info->channel_count = sf_info.channels;
				info->format = DECODER_FORMAT_F32;
				info->is_complex = false;

				return decoder;
			}

			sf_close(sndfile);
		}

		ROMemoryStream_Destroy(memory_stream);
	}

	return NULL;
}

void Decoder_libSndfile_Destroy(Decoder *decoder)
{
	sf_close(decoder->sndfile);
	ROMemoryStream_Destroy(decoder->memory_stream);
	free(decoder);
}

void Decoder_libSndfile_Rewind(Decoder *decoder)
{
	sf_seek(decoder->sndfile, 0, SEEK_SET);
}

size_t Decoder_libSndfile_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do)
{
	return sf_readf_float(decoder->sndfile, buffer, frames_to_do);
}
