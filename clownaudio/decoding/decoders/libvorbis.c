/*
 *  (C) 2019-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include "libvorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "common.h"
#include "memory_stream.h"

struct Decoder
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

Decoder* Decoder_libVorbis_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	(void)loop;	// This is ignored in simple decoders

	Decoder *decoder = NULL;

	ROMemoryStream *memory_stream = ROMemoryStream_Create(data, data_size);

	if (memory_stream != NULL)
	{
		OggVorbis_File vorbis_file;

		if (ov_open_callbacks(memory_stream, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
		{
			decoder = malloc(sizeof(Decoder));

			if (decoder != NULL)
			{
				vorbis_info *v_info = ov_info(&vorbis_file, -1);

				decoder->vorbis_file = vorbis_file;
				decoder->channel_count = v_info->channels;

				info->sample_rate = v_info->rate;
				info->channel_count = v_info->channels;
				info->format = DECODER_FORMAT_F32;
				info->is_complex = false;
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

void Decoder_libVorbis_Destroy(Decoder *decoder)
{
	ov_clear(&decoder->vorbis_file);
	free(decoder);
}

void Decoder_libVorbis_Rewind(Decoder *decoder)
{
	ov_time_seek(&decoder->vorbis_file, 0);
}

size_t Decoder_libVorbis_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do)
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
