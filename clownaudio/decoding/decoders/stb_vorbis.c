/*
 *  (C) 2019-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#include "stb_vorbis.h"

#include <stdbool.h>
#include <stddef.h>

#define STB_VORBIS_IMPLEMENTATION
#define STB_VORBIS_NO_STDIO
#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_NO_INTEGER_CONVERSION

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include "libs/stb_vorbis.c"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#include "common.h"

Decoder* Decoder_STB_Vorbis_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	(void)loop;	// This is ignored in simple decoders

	stb_vorbis *instance = stb_vorbis_open_memory(data, data_size, NULL, NULL);

	if (instance != NULL)
	{
		const stb_vorbis_info vorbis_info = stb_vorbis_get_info(instance);

		info->sample_rate = vorbis_info.sample_rate;
		info->channel_count = vorbis_info.channels;
		info->format = DECODER_FORMAT_F32;
		info->is_complex = false;
	}

	return (Decoder*)instance;
}

void Decoder_STB_Vorbis_Destroy(Decoder *decoder)
{
	stb_vorbis_close((stb_vorbis*)decoder);
}

void Decoder_STB_Vorbis_Rewind(Decoder *decoder)
{
	stb_vorbis_seek_start((stb_vorbis*)decoder);
}

size_t Decoder_STB_Vorbis_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do)
{
	stb_vorbis *instance = (stb_vorbis*)decoder;

	return stb_vorbis_get_samples_float_interleaved(instance, instance->channels, buffer, frames_to_do * instance->channels);
}
