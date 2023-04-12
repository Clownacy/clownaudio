/*
Copyright (c) 2023 Ned Loynd

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/

#include "oswrapper_audio.h"

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>
#include <stdlib.h>

#define OSWRAPPER_AUDIO_NO_LOAD_FROM_PATH
#define OSWRAPPER_AUDIO_IMPLEMENTATION
#include "libs/OSWrapper/oswrapper_audio.h"

#include "common.h"

bool is_oswrapper_audio_loaded = false;

void* Decoder_OSWRAPPER_AUDIO_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec)
{
	if (!is_oswrapper_audio_loaded)
		return NULL;

	OSWrapper_audio_spec *audio_spec = (OSWrapper_audio_spec*)calloc(1, sizeof(OSWrapper_audio_spec));

	(void)loop;

	if (audio_spec != NULL)
	{
#ifdef CLOWNAUDIO_OSWRAPPER_AUDIO_HINT_RESAMPLE
		audio_spec->sample_rate = wanted_spec->sample_rate;
#endif
		audio_spec->channel_count = wanted_spec->channel_count;
		audio_spec->bits_per_channel = 16;
		audio_spec->audio_type = OSWRAPPER_AUDIO_FORMAT_PCM_INTEGER;

		if (oswrapper_audio_load_from_memory(data, data_size, audio_spec))
		{
			if (audio_spec->audio_type == OSWRAPPER_AUDIO_FORMAT_PCM_INTEGER && audio_spec->bits_per_channel == 16)
			{
				spec->sample_rate = audio_spec->sample_rate;
				spec->channel_count = audio_spec->channel_count;
				spec->is_complex = false;

				return audio_spec;
			}

			oswrapper_audio_free_context(audio_spec);
		}

		free(audio_spec);
	}

	return NULL;
}

void Decoder_OSWRAPPER_AUDIO_Destroy(void *decoder_void)
{
	OSWrapper_audio_spec *audio_spec = (OSWrapper_audio_spec*)decoder_void;

	oswrapper_audio_free_context(audio_spec);

	free(audio_spec);
}

void Decoder_OSWRAPPER_AUDIO_Rewind(void *decoder_void)
{
	OSWrapper_audio_spec *audio_spec = (OSWrapper_audio_spec*)decoder_void;

	oswrapper_audio_rewind(audio_spec);
}

size_t Decoder_OSWRAPPER_AUDIO_GetSamples(void *decoder_void, short *buffer, size_t frames_to_do)
{
	OSWrapper_audio_spec *audio_spec = (OSWrapper_audio_spec*)decoder_void;

	return oswrapper_audio_get_samples(audio_spec, buffer, frames_to_do);
}
