#include "dr_wav.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO

#include "libs/dr_wav.h"

#include "../common.h"

Decoder_DR_WAV* Decoder_DR_WAV_Create(const unsigned char *data, size_t data_size, DecoderInfo *info)
{
	drwav *instance = drwav_open_memory(data, data_size);

	if (instance != NULL)
	{
		info->sample_rate = instance->sampleRate;
		info->channel_count = instance->channels;
		info->format = DECODER_FORMAT_F32;
	}

	return (Decoder_DR_WAV*)instance;
}

void Decoder_DR_WAV_Destroy(Decoder_DR_WAV *decoder)
{
	drwav_uninit((drwav*)decoder);
}

void Decoder_DR_WAV_Rewind(Decoder_DR_WAV *decoder)
{
	drwav_seek_to_pcm_frame((drwav*)decoder, 0);
}

size_t Decoder_DR_WAV_GetSamples(Decoder_DR_WAV *decoder, void *buffer, size_t frames_to_do)
{
	return drwav_read_pcm_frames_f32((drwav*)decoder, frames_to_do, buffer);
}
