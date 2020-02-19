#include "libopenmpt.h"

#include <stdbool.h>
#include <stddef.h>

#include <libopenmpt/libopenmpt.h>

#include "common.h"

#define SAMPLE_RATE 48000
#define CHANNEL_COUNT 2

Decoder* Decoder_libOpenMPT_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	openmpt_module *module = openmpt_module_create_from_memory2(data, data_size, openmpt_log_func_silent, NULL, openmpt_error_func_ignore, NULL, NULL, NULL, NULL);

	if (module != NULL)
	{
		info->sample_rate = SAMPLE_RATE;
		info->channel_count = CHANNEL_COUNT;
		info->format = DECODER_FORMAT_F32;
		info->complex = true;

		if (loop)
			openmpt_module_set_repeat_count(module, -1);
	}

	return (Decoder*)module;
}

void Decoder_libOpenMPT_Destroy(Decoder *decoder)
{
	openmpt_module_destroy((openmpt_module*)decoder);
}

void Decoder_libOpenMPT_Rewind(Decoder *decoder)
{
	openmpt_module_set_position_seconds((openmpt_module*)decoder, 0);
}

size_t Decoder_libOpenMPT_GetSamples(Decoder *decoder, void *buffer_void, size_t frames_to_do)
{
	float *buffer = buffer_void;

	size_t frames_done = 0;

	while (frames_done != frames_to_do)
	{
		size_t frames = openmpt_module_read_interleaved_float_stereo((openmpt_module*)decoder, SAMPLE_RATE, frames_to_do - frames_done, &buffer[frames_done * CHANNEL_COUNT]);

		if (frames == 0)
			break;

		frames_done += frames;
	}

	return frames_done;
}
