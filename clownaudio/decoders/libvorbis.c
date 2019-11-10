#include "libvorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "common.h"
#include "memory_stream.h"

struct Decoder_libVorbis
{
	DecoderData *data;
	OggVorbis_File vorbis_file;
	bool loops;
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

Decoder_libVorbis* Decoder_libVorbis_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	(void)sample_rate;
	(void)channel_count;

	Decoder_libVorbis *decoder = NULL;

	if (data != NULL)
	{
		ROMemoryStream *memory_stream = ROMemoryStream_Create(data->file_buffer, data->file_size);

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
					decoder->data = data;
					decoder->loops = loops;

					info->sample_rate = v_info->rate;
					info->channel_count = v_info->channels;
					info->format = DECODER_FORMAT_F32;
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
	}

	return decoder;
}

void Decoder_libVorbis_Destroy(Decoder_libVorbis *decoder)
{
	if (decoder != NULL)
	{
		ov_clear(&decoder->vorbis_file);
		free(decoder);
	}
}

void Decoder_libVorbis_Rewind(Decoder_libVorbis *decoder)
{
	ov_time_seek(&decoder->vorbis_file, 0);
}

static unsigned long ReadFloats(Decoder_libVorbis *decoder, float *buffer, unsigned long frames_to_do)
{
	float **float_buffer;
	long frames_read = ov_read_float(&decoder->vorbis_file, &float_buffer, frames_to_do, NULL);

	for (long i = 0; i < frames_read; ++i)
		for (unsigned int j = 0; j < decoder->channel_count; ++j)
			*buffer++ = float_buffer[j][i];

	return frames_read;
}

unsigned long Decoder_libVorbis_GetSamples(Decoder_libVorbis *decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done = 0;

	for (;;)
	{
		unsigned long frames = ReadFloats(decoder, &buffer[frames_done * decoder->channel_count], frames_to_do - frames_done);

		if (frames == 0)
		{
			if (decoder->loops)
				Decoder_libVorbis_Rewind(decoder);
			else
				break;
		}

		frames_done += frames;

		if (frames_done == frames_to_do)
			break;
	}


	return frames_done;
}
