#include "dr_flac.h"

#include <stdbool.h>
#include <stddef.h>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO

#include "libs/dr_flac.h"

#include "common.h"

Decoder* Decoder_DR_FLAC_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	(void)loop;	// This is ignored in simple decoders

	drflac *backend = drflac_open_memory(data, data_size, NULL);

	if (backend != NULL)
	{
		info->sample_rate = backend->sampleRate;
		info->channel_count = backend->channels;
		info->format = DECODER_FORMAT_S32;
		info->complex = false;
	}

	return (Decoder*)backend;
}

void Decoder_DR_FLAC_Destroy(Decoder *decoder)
{
	drflac_close((drflac*)decoder);
}

void Decoder_DR_FLAC_Rewind(Decoder *decoder)
{
	drflac_seek_to_pcm_frame((drflac*)decoder, 0);
}

size_t Decoder_DR_FLAC_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do)
{
	return drflac_read_pcm_frames_s32((drflac*)decoder, frames_to_do, buffer);
}
