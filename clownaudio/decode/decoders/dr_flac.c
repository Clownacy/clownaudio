#include "dr_flac.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_STDIO
#define DR_FLAC_NO_OGG

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif
#include "libs/dr_flac.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "../../common.h"

Decoder_DR_FLAC* Decoder_DR_FLAC_Create(const unsigned char *data, size_t data_size, DecoderInfo *info)
{
	drflac *backend = drflac_open_memory(data, data_size);

	if (backend != NULL)
	{
		info->sample_rate = backend->sampleRate;
		info->channel_count = backend->channels;
		info->format = DECODER_FORMAT_S32;
	}

	return (Decoder_DR_FLAC*)backend;
}

void Decoder_DR_FLAC_Destroy(Decoder_DR_FLAC *decoder)
{
	drflac *backend = (drflac*)decoder;

	drflac_close(backend);
}

void Decoder_DR_FLAC_Rewind(Decoder_DR_FLAC *decoder)
{
	drflac *backend = (drflac*)decoder;

	drflac_seek_to_pcm_frame(backend, 0);
}

size_t Decoder_DR_FLAC_GetSamples(Decoder_DR_FLAC *decoder, void *buffer, size_t frames_to_do)
{
	drflac *backend = (drflac*)decoder;

	return drflac_read_pcm_frames_s32(backend, frames_to_do, buffer);
}
