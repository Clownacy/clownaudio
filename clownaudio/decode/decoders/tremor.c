#include "tremor.h"

#include <stddef.h>
#include <stdlib.h>

#include <tremor/ivorbisfile.h>

#include "../common.h"
#include "../memory_stream.h"

struct Decoder_Tremor
{
	OggVorbis_File vorbis_file;
	unsigned int bytes_per_frame;
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

Decoder_Tremor* Decoder_Tremor_Create(const unsigned char *data, size_t data_size, DecoderInfo *info)
{
	Decoder_Tremor *decoder = NULL;

	if (data[0] == 'O' && data[1] == 'g' && data[2] == 'g' && data[3] == 'S')	// Detect .ogg files (because Tremor has a bad habit of trying to process .flac files, and corrupting the stack)
	{
		ROMemoryStream *memory_stream = ROMemoryStream_Create(data, data_size);

		if (memory_stream != NULL)
		{
			OggVorbis_File vorbis_file;

			if (ov_open_callbacks(memory_stream, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
			{
				vorbis_info *v_info = ov_info(&vorbis_file, -1);

				decoder = malloc(sizeof(Decoder_Tremor));

				if (decoder != NULL)
				{
					decoder->vorbis_file = vorbis_file;
					decoder->bytes_per_frame = v_info->channels * sizeof(short);

					info->sample_rate = v_info->rate;
					info->channel_count = v_info->channels;
					info->format = DECODER_FORMAT_S16;	// Note to self: Tremor uses the 'short' type internally, which in theory could be more than two bytes
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

void Decoder_Tremor_Destroy(Decoder_Tremor *decoder)
{
	ov_clear(&decoder->vorbis_file);
	free(decoder);
}

void Decoder_Tremor_Rewind(Decoder_Tremor *decoder)
{
	ov_time_seek(&decoder->vorbis_file, 0);
}

unsigned long Decoder_Tremor_GetSamples(Decoder_Tremor *decoder, void *buffer, unsigned long frames_to_do)
{
	return ov_read(&decoder->vorbis_file, buffer, frames_to_do * decoder->bytes_per_frame, NULL) / decoder->bytes_per_frame;
}
