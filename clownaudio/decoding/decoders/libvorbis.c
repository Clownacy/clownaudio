#include "libvorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "common.h"
#include "memory_stream.h"

struct Decoder_libVorbis
{
	OggVorbis_File vorbis_file;
	unsigned int channel_count;
};

static size_t fread_wrapper(void *output, size_t size, size_t count, void *file)
{
	return ROMemoryStream_Read(file, output, size, count);
}

static int fseek_wrapper(void *file, ogg_int64_t offset, int origin)
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

	return (ROMemoryStream_SetPosition(file, offset, memory_stream_origin) ? 0 : -1);
}

static int fclose_wrapper(void *file)
{
	ROMemoryStream_Destroy(file);

	return 0;
}

static long ftell_wrapper(void *file)
{
	return ROMemoryStream_GetPosition(file);
}

static const ov_callbacks ov_callback_memory = {
	fread_wrapper,
	fseek_wrapper,
	fclose_wrapper,
	ftell_wrapper
};

Decoder_libVorbis* Decoder_libVorbis_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	(void)loop;	// This is ignored in simple decoders

	Decoder_libVorbis *decoder = NULL;

	ROMemoryStream *memory_stream = ROMemoryStream_Create(data, data_size);

	if (memory_stream != NULL)
	{
		OggVorbis_File vorbis_file;

		if (ov_open_callbacks(memory_stream, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			decoder = malloc(sizeof(Decoder_libVorbis));

			if (decoder != NULL)
			{
				vorbis_info *v_info = ov_info(&vorbis_file, -1);

				decoder->vorbis_file = vorbis_file;
				decoder->channel_count = v_info->channels;

				info->sample_rate = v_info->rate;
				info->channel_count = v_info->channels;
				info->format = DECODER_FORMAT_F32;
				info->complex = false;
			}
			else
			{
				ov_clear(&vorbis_file);
			}
		}
		else
		{
			ROMemoryStream_Destroy(memory_stream);
		}
	}

	return decoder;
}

void Decoder_libVorbis_Destroy(Decoder_libVorbis *decoder)
{
	ov_clear(&decoder->vorbis_file);
	free(decoder);
}

void Decoder_libVorbis_Rewind(Decoder_libVorbis *decoder)
{
	ov_time_seek(&decoder->vorbis_file, 0);
}

size_t Decoder_libVorbis_GetSamples(Decoder_libVorbis *decoder, void *buffer, size_t frames_to_do)
{
	float **source_buffer;
	size_t frames_done = ov_read_float(&decoder->vorbis_file, &source_buffer, frames_to_do, NULL);

	for (unsigned int i = 0; i < decoder->channel_count; ++i)
	{
		float *source_buffer_pointer = source_buffer[i];
		float *destination_buffer_pointer = &((float*)buffer)[i];

		for (size_t j = 0; j < frames_done; ++j)
		{
			*destination_buffer_pointer = *source_buffer_pointer++;
			destination_buffer_pointer += decoder->channel_count;
		}
	}

	return frames_done;
}
