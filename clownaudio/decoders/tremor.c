#include "tremor.h"

#include <stddef.h>
#include <stdlib.h>

#include <tremor/ivorbisfile.h>

#include "common.h"
#include "memory_file.h"

struct Decoder_Tremor
{
	DecoderData *data;
	OggVorbis_File vorbis_file;
	bool loops;
	unsigned int bytes_per_frame;
	unsigned int channel_count;
};

static size_t MemoryFile_fread_wrapper(void *output, size_t size, size_t count, void *file)
{
	return MemoryFile_fread(output, size, count, file);
}

static int MemoryFile_fseek_wrapper(void *file, ogg_int64_t offset, int origin)
{
	return MemoryFile_fseek(file, offset, origin);
}

static int MemoryFile_fclose_wrapper(void *file)
{
	return MemoryFile_fclose(file);
}

static long MemoryFile_ftell_wrapper(void *file)
{
	return MemoryFile_ftell(file);
}

static const ov_callbacks ov_callback_memory = {
	MemoryFile_fread_wrapper,
	MemoryFile_fseek_wrapper,
	MemoryFile_fclose_wrapper,
	MemoryFile_ftell_wrapper
};

Decoder_Tremor* Decoder_Tremor_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned channel_count, DecoderInfo *info)
{
	(void)sample_rate;
	(void)channel_count;

	Decoder_Tremor *decoder = NULL;

	if (data != NULL)
	{
		MemoryFile *file = MemoryFile_fopen_from((unsigned char*)data->file_buffer, data->file_size, false);

		if (file != NULL)
		{
			OggVorbis_File vorbis_file;

			if (ov_open_callbacks(file, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
			{
				vorbis_info *v_info = ov_info(&vorbis_file, -1);

				decoder = malloc(sizeof(Decoder_Tremor));

				if (decoder != NULL)
				{
					decoder->data = data;
					decoder->vorbis_file = vorbis_file;
					decoder->loops = loops;
					decoder->channel_count = v_info->channels;
					decoder->bytes_per_frame = v_info->channels * sizeof(short);

					info->sample_rate = v_info->rate;
					info->channel_count = v_info->channels;
					info->decoded_size = ov_pcm_total(&vorbis_file, -1) * decoder->bytes_per_frame;
					info->format = DECODER_FORMAT_S16;
				}
				else
				{
					ov_clear(&vorbis_file);
				}
			}
			else
			{
				MemoryFile_fclose(file);
			}
		}
	}

	return decoder;
}

void Decoder_Tremor_Destroy(Decoder_Tremor *decoder)
{
	if (decoder)
	{
		ov_clear(&decoder->vorbis_file);
		free(decoder);
	}
}

void Decoder_Tremor_Rewind(Decoder_Tremor *decoder)
{
	ov_time_seek(&decoder->vorbis_file, 0);
}

unsigned long Decoder_Tremor_GetSamples(Decoder_Tremor *decoder, void *buffer_void, unsigned long frames_to_do)
{
	char *buffer = buffer_void;

	const unsigned long bytes_to_do = frames_to_do * decoder->bytes_per_frame;

	unsigned long bytes_done_total = 0;

	for (unsigned long bytes_done; bytes_done_total != bytes_to_do; bytes_done_total += bytes_done)
	{
		bytes_done = ov_read(&decoder->vorbis_file, buffer + bytes_done_total, bytes_to_do - bytes_done_total, NULL);

		if (bytes_done == 0)
		{
			if (decoder->loops)
				Decoder_Tremor_Rewind(decoder);
			else
				break;
		}
	}

	return bytes_done_total / decoder->bytes_per_frame;
}
