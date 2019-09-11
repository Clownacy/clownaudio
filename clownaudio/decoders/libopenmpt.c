#include "libopenmpt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <libopenmpt/libopenmpt.h>

#include "common.h"
#include "memory_file.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

struct DecoderData_libOpenMPT
{
	unsigned char *file_buffer;
	size_t file_size;
};

struct Decoder_libOpenMPT
{
	openmpt_module *module;
};

DecoderData_libOpenMPT* Decoder_libOpenMPT_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_libOpenMPT *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_libOpenMPT));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_libOpenMPT_UnloadData(DecoderData_libOpenMPT *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_libOpenMPT* Decoder_libOpenMPT_Create(DecoderData_libOpenMPT *data, bool loops, DecoderInfo *info)
{
	Decoder_libOpenMPT *decoder = NULL;

	if (data)
	{
		openmpt_module *module = openmpt_module_create_from_memory2(data->file_buffer, data->file_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);

		if (module)
		{
			decoder = malloc(sizeof(Decoder_libOpenMPT));

			decoder->module = module;

			info->sample_rate = SAMPLE_RATE;
			info->channel_count = CHANNEL_COUNT;
			info->format = DECODER_FORMAT_F32;

			if (loops)
				openmpt_module_set_repeat_count(decoder->module, -1);
		}
	}

	return decoder;
}

void Decoder_libOpenMPT_Destroy(Decoder_libOpenMPT *decoder)
{
	if (decoder)
	{
		openmpt_module_destroy(decoder->module);
		free(decoder);
	}
}

void Decoder_libOpenMPT_Rewind(Decoder_libOpenMPT *decoder)
{
	openmpt_module_set_position_seconds(decoder->module, 0);
}

unsigned long Decoder_libOpenMPT_GetSamples(Decoder_libOpenMPT *decoder, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = openmpt_module_read_interleaved_float_stereo(decoder->module, SAMPLE_RATE, frames_to_do - frames_done_total, buffer + (frames_done_total * CHANNEL_COUNT));

		if (frames_done == 0)
			break;
	}

	return frames_done_total;
}
