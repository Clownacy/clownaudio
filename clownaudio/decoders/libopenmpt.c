#include "libopenmpt.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <libopenmpt/libopenmpt.h>

#include "common.h"

struct Decoder_libOpenMPT
{
	openmpt_module *module;
	unsigned long sample_rate;
	unsigned int channel_count;
};

Decoder_libOpenMPT* Decoder_libOpenMPT_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info)
{
	Decoder_libOpenMPT *decoder = NULL;

	if (data != NULL)
	{
		openmpt_module *module = openmpt_module_create_from_memory2(data->file_buffer, data->file_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);

		if (module != NULL)
		{
			decoder = malloc(sizeof(Decoder_libOpenMPT));

			if (decoder != NULL)
			{
				decoder->module = module;
				decoder->sample_rate = sample_rate;
				decoder->channel_count = channel_count;

				info->sample_rate = sample_rate;
				info->channel_count = channel_count;
				info->format = DECODER_FORMAT_F32;

				if (loops)
					openmpt_module_set_repeat_count(decoder->module, -1);
			}
			else
			{
				openmpt_module_destroy(module);
			}
		}
	}

	return decoder;
}

void Decoder_libOpenMPT_Destroy(Decoder_libOpenMPT *decoder)
{
	if (decoder != NULL)
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
		frames_done = openmpt_module_read_interleaved_float_stereo(decoder->module, decoder->sample_rate, frames_to_do - frames_done_total, buffer + (frames_done_total * decoder->channel_count));

		if (frames_done == 0)
			break;
	}

	return frames_done_total;
}
