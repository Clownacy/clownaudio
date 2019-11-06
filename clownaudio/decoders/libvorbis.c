#include "libvorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <vorbis/vorbisfile.h>

#include "common.h"
#include "memory_file.h"

struct Decoder_libVorbis
{
	DecoderData *data;
	OggVorbis_File vorbis_file;
	bool loops;
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

Decoder_libVorbis* Decoder_libVorbis_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	(void)sample_rate;
	(void)channel_count;

	Decoder_libVorbis *decoder = NULL;

	if (data != NULL)
	{
		MemoryFile *memory_file = MemoryFile_fopen_from((unsigned char*)data->file_buffer, data->file_size, false);

		OggVorbis_File vorbis_file;

		if (ov_open_callbacks(memory_file, &vorbis_file, NULL, 0, ov_callback_memory) == 0)
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
				info->decoded_size = ov_pcm_total(&vorbis_file, -1) * v_info->channels * sizeof(float);
				info->format = DECODER_FORMAT_F32;
			}
			else
			{
				ov_clear(&vorbis_file);
			}
		}
		else
		{
			MemoryFile_fclose(memory_file);
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

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = ReadFloats(decoder, buffer + (frames_done_total * decoder->channel_count), frames_to_do - frames_done_total);

		if (frames_done == 0)
		{
			if (decoder->loops)
				Decoder_libVorbis_Rewind(decoder);
			else
				break;
		}
	}


	return frames_done_total;
}
