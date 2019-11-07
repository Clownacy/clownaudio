#include "libsndfile.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "common.h"
#include "../memory_stream.h"

struct Decoder_libSndfile
{
	DecoderData *data;
	ROMemoryStream *memory_stream;
	SNDFILE *sndfile;
	DecoderFormat format;
	bool loops;
	unsigned int channel_count;
};

static sf_count_t MemoryFile_fread_wrapper(void *output, sf_count_t count, void *user)
{
	return ROMemoryStream_Read(user, output, 1, count);
}

static sf_count_t MemoryFile_fseek_wrapper(sf_count_t offset, int origin, void *user)
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

static sf_count_t MemoryFile_ftell_wrapper(void *user)
{
	return ROMemoryStream_GetPosition(user);
}

static sf_count_t MemoryFile_GetSize(void *user)
{
	const sf_count_t old_offset = MemoryFile_ftell_wrapper(user);

	MemoryFile_fseek_wrapper(0, SEEK_END, user);
	const sf_count_t size = MemoryFile_ftell_wrapper(user);

	MemoryFile_fseek_wrapper(old_offset, SEEK_SET, user);

	return size;
}

static SF_VIRTUAL_IO sfvirtual = {
	MemoryFile_GetSize,
	MemoryFile_fseek_wrapper,
	MemoryFile_fread_wrapper,
	NULL,
	MemoryFile_ftell_wrapper
};

Decoder_libSndfile* Decoder_libSndfile_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	(void)sample_rate;
	(void)channel_count;

	Decoder_libSndfile *decoder = NULL;

	if (data != NULL)
	{
		ROMemoryStream *memory_stream = ROMemoryStream_Create(data->file_buffer, data->file_size);

		if (memory_stream != NULL)
		{
			SF_INFO sf_info;
			memset(&sf_info, 0, sizeof(SF_INFO));

			SNDFILE *sndfile = sf_open_virtual(&sfvirtual, SFM_READ, &sf_info, memory_stream);

			if (sndfile != NULL)
			{
				decoder = malloc(sizeof(Decoder_libSndfile));

				if (decoder != NULL)
				{
					decoder->data = data;
					decoder->sndfile = sndfile;
					decoder->memory_stream = memory_stream;
					decoder->channel_count = sf_info.channels;
					decoder->loops = loops;

					info->sample_rate = sf_info.samplerate;
					info->channel_count = sf_info.channels;
					info->format = DECODER_FORMAT_F32;
				}
				else
				{
					sf_close(sndfile);
					ROMemoryStream_Destroy(memory_stream);
				}
			}
			else
			{
				ROMemoryStream_Destroy(memory_stream);
			}
		}
	}

	return decoder;
}

void Decoder_libSndfile_Destroy(Decoder_libSndfile *decoder)
{
	if (decoder != NULL)
	{
		sf_close(decoder->sndfile);
		ROMemoryStream_Destroy(decoder->memory_stream);
		free(decoder);
	}
}

void Decoder_libSndfile_Rewind(Decoder_libSndfile *decoder)
{
	sf_seek(decoder->sndfile, 0, SEEK_SET);
}

unsigned long Decoder_libSndfile_GetSamples(Decoder_libSndfile *decoder, void *output_buffer_void, unsigned long frames_to_do)
{
	float *output_buffer = output_buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total < frames_to_do; frames_done_total += frames_done)
	{
		frames_done = sf_readf_float(decoder->sndfile, output_buffer + (frames_done_total * decoder->channel_count), frames_to_do - frames_done_total);

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (decoder->loops)
				Decoder_libSndfile_Rewind(decoder);
			else
				break;
		}
	}

	return frames_done_total;
}
