#include "libopus.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <opus/opusfile.h>

#include "common.h"

Decoder_libOpus* Decoder_libOpus_Create(const unsigned char *data, size_t data_size, DecoderInfo *info)
{
	OggOpusFile *backend = op_open_memory(data, data_size, NULL);

	if (backend != NULL)
	{
		info->sample_rate = 48000;  // libopusfile always outputs at 48kHz (https://opus-codec.org/docs/opusfile_api-0.7/structOpusHead.html#a73b80a913eca33d829f1667caee80d9e)
		info->channel_count = 2;    // We use op_read_float_stereo, so libopusfile will handle conversion if it needs to
		info->format = DECODER_FORMAT_F32;
	}

	return (Decoder_libOpus*)backend;
}

void Decoder_libOpus_Destroy(Decoder_libOpus *decoder)
{
	op_free((OggOpusFile*)decoder);
}

void Decoder_libOpus_Rewind(Decoder_libOpus *decoder)
{
	op_pcm_seek((OggOpusFile*)decoder, 0);
}

size_t Decoder_libOpus_GetSamples(Decoder_libOpus *decoder, void *buffer, size_t frames_to_do)
{
	return op_read_float_stereo((OggOpusFile*)decoder, buffer, frames_to_do * 2);	// You tell *me* why that last parameter is in samples and not frames
}
