#include "stb_vorbis.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-value"
#endif
#include "libs/stb_vorbis.c"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "../common.h"

Decoder_STB_Vorbis* Decoder_STB_Vorbis_Create(const unsigned char *data, size_t data_size, DecoderInfo *info)
{
	stb_vorbis *instance = stb_vorbis_open_memory(data, data_size, NULL, NULL);

	if (instance != NULL)
	{
		const stb_vorbis_info vorbis_info = stb_vorbis_get_info(instance);

		info->sample_rate = vorbis_info.sample_rate;
		info->channel_count = vorbis_info.channels;
		info->format = DECODER_FORMAT_F32;
	}

	return (Decoder_STB_Vorbis*)instance;
}

void Decoder_STB_Vorbis_Destroy(Decoder_STB_Vorbis *decoder)
{
	stb_vorbis_close((stb_vorbis*)decoder);
	free(decoder);
}

void Decoder_STB_Vorbis_Rewind(Decoder_STB_Vorbis *decoder)
{
	stb_vorbis_seek_start((stb_vorbis*)decoder);
}

unsigned long Decoder_STB_Vorbis_GetSamples(Decoder_STB_Vorbis *decoder, void *buffer, unsigned long frames_to_do)
{
	stb_vorbis *instance = (stb_vorbis*)decoder;

	return stb_vorbis_get_samples_float_interleaved(instance, instance->channels, buffer, frames_to_do * instance->channels);
}
