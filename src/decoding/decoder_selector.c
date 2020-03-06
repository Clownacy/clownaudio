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

#include "decoder_selector.h"

#include <stddef.h>
#include <stdlib.h>

#include "bool.h"

#include "decoders/common.h"
#include "predecoder.h"

#ifdef USE_LIBVORBIS
#include "decoders/libvorbis.h"
#endif
#ifdef USE_STB_VORBIS
#include "decoders/stb_vorbis.h"
#endif
#ifdef USE_LIBFLAC
#include "decoders/libflac.h"
#endif
#ifdef USE_DR_FLAC
#include "decoders/dr_flac.h"
#endif
#ifdef USE_DR_WAV
#include "decoders/dr_wav.h"
#endif
#ifdef USE_LIBOPUS
#include "decoders/libopus.h"
#endif
#ifdef USE_LIBSNDFILE
#include "decoders/libsndfile.h"
#endif
#ifdef USE_LIBXMPLITE
#include "decoders/libxmp-lite.h"
#endif
#ifdef USE_LIBOPENMPT
#include "decoders/libopenmpt.h"
#endif
#ifdef USE_SNES_SPC
#include "decoders/snes_spc.h"
#endif
#ifdef USE_PXTONE
#include "decoders/pxtone.h"
#include "decoders/pxtone_noise.h"
#endif

#define DECODER_FUNCTIONS(name) \
{ \
	(void*(*)(const unsigned char*,size_t,CA_BOOL,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(size_t(*)(void*,void*,size_t))Decoder_##name##_GetSamples \
}

typedef enum DecoderType
{
	DECODER_TYPE_PREDECODER,
	DECODER_TYPE_COMPLEX,
	DECODER_TYPE_SIMPLE
} DecoderType;

typedef struct DecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, CA_BOOL loop, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} DecoderFunctions;

struct DecoderSelectorData
{
	const unsigned char *file_buffer;
	size_t file_size;
	DecoderType decoder_type;
	const DecoderFunctions *decoder_functions;
	PredecoderData *predecoder_data;
	size_t size_of_frame;
};

struct DecoderSelector
{
	void *decoder;
	DecoderSelectorData *data;
	CA_BOOL loop;
};

static const DecoderFunctions decoder_function_list[] = {
#ifdef USE_LIBVORBIS
	DECODER_FUNCTIONS(libVorbis),
#endif
#ifdef USE_STB_VORBIS
	DECODER_FUNCTIONS(STB_Vorbis),
#endif
#ifdef USE_LIBFLAC
	DECODER_FUNCTIONS(libFLAC),
#endif
#ifdef USE_DR_FLAC
	DECODER_FUNCTIONS(DR_FLAC),
#endif
#ifdef USE_DR_WAV
	DECODER_FUNCTIONS(DR_WAV),
#endif
#ifdef USE_LIBOPUS
	DECODER_FUNCTIONS(libOpus),
#endif
#ifdef USE_LIBSNDFILE
	DECODER_FUNCTIONS(libSndfile),
#endif
#ifdef USE_LIBOPENMPT
	DECODER_FUNCTIONS(libOpenMPT),
#endif
#ifdef USE_LIBXMPLITE
	DECODER_FUNCTIONS(libXMPLite),
#endif
#ifdef USE_SNES_SPC
	DECODER_FUNCTIONS(SNES_SPC),
#endif
#ifdef USE_PXTONE
	DECODER_FUNCTIONS(PxTone),
#endif
#ifdef USE_PXTONE
	DECODER_FUNCTIONS(PxToneNoise),
#endif
};

static const DecoderFunctions predecoder_functions = {
	NULL,
	(void(*)(void*))Predecoder_Destroy,
	(void(*)(void*))Predecoder_Rewind,
	(size_t(*)(void*,void*,size_t))Predecoder_GetSamples
};

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *file_buffer, size_t file_size, CA_BOOL predecode)
{
	DecoderType decoder_type;
	const DecoderFunctions *decoder_functions = NULL;
	PredecoderData *predecoder_data = NULL;

	DecoderInfo info;

	/* Figure out what format this sound is */
	size_t i;
	for (i = 0; i < sizeof(decoder_function_list) / sizeof(decoder_function_list[0]); ++i)
	{
		void *decoder = decoder_function_list[i].Create(file_buffer, file_size, CA_FALSE, &info);

		if (decoder != NULL)
		{
			decoder_type = info.is_complex ? DECODER_TYPE_COMPLEX : DECODER_TYPE_SIMPLE;
			decoder_functions = &decoder_function_list[i];

			if (decoder_type == DECODER_TYPE_SIMPLE && predecode)
			{
				predecoder_data = Predecoder_DecodeData(&info, decoder, decoder_functions[i].GetSamples);

				if (predecoder_data != NULL)
				{
					decoder_type = DECODER_TYPE_PREDECODER;
					decoder_functions = &predecoder_functions;
				}
			}

			decoder_function_list[i].Destroy(decoder);

			break;
		}
	}

	if (decoder_functions != NULL)
	{
		DecoderSelectorData *data = malloc(sizeof(DecoderSelectorData));

		if (data != NULL)
		{
			data->file_buffer = file_buffer;
			data->file_size = file_size;
			data->decoder_type = decoder_type;
			data->decoder_functions = decoder_functions;
			data->predecoder_data = predecoder_data;
			data->size_of_frame = info.channel_count;

			switch (info.format)
			{
				case DECODER_FORMAT_S16:
					data->size_of_frame *= 2;
					break;

				case DECODER_FORMAT_S32:
				case DECODER_FORMAT_F32:
					data->size_of_frame *= 4;
					break;
			}

			return data;
		}
	}

	if (predecoder_data != NULL)
		Predecoder_UnloadData(predecoder_data);

	return NULL;
}

void DecoderSelector_UnloadData(DecoderSelectorData *data)
{
	if (data->predecoder_data != NULL)
		Predecoder_UnloadData(data->predecoder_data);

	free(data);
}

DecoderSelector* DecoderSelector_Create(DecoderSelectorData *data, CA_BOOL loop, DecoderInfo *info)
{
	DecoderSelector *selector = malloc(sizeof(DecoderSelector));

	if (selector != NULL)
	{
		if (data->decoder_type == DECODER_TYPE_PREDECODER)
			selector->decoder = Predecoder_Create(data->predecoder_data, loop, info);
		else
			selector->decoder = data->decoder_functions->Create(data->file_buffer, data->file_size, loop, info);

		if (selector->decoder != NULL)
		{
			selector->data = data;
			selector->loop = loop;
			return selector;
		}

		free(selector);
	}

	return NULL;
}

void DecoderSelector_Destroy(DecoderSelector *selector)
{
	selector->data->decoder_functions->Destroy(selector->decoder);
	free(selector);
}

void DecoderSelector_Rewind(DecoderSelector *selector)
{
	selector->data->decoder_functions->Rewind(selector->decoder);
}

size_t DecoderSelector_GetSamples(DecoderSelector *selector, void *buffer, size_t frames_to_do)
{
	size_t frames_done = 0;

	switch (selector->data->decoder_type)
	{
		default:
			return 0;

		case DECODER_TYPE_PREDECODER:
			return Predecoder_GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_COMPLEX:
			return selector->data->decoder_functions->GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_SIMPLE:
			/* Handle looping here, since the simple decoders don't do it by themselves */
			while (frames_done != frames_to_do)
			{
				const size_t frames = selector->data->decoder_functions->GetSamples(selector->decoder, (char*)buffer + frames_done * selector->data->size_of_frame, frames_to_do - frames_done);

				if (frames == 0)
				{
					if (selector->loop)
						selector->data->decoder_functions->Rewind(selector->decoder);
					else
						break;
				}

				frames_done += frames;
			}

			return frames_done;
	}
}

void DecoderSelector_SetLoop(DecoderSelector *selector, CA_BOOL loop)
{
	switch (selector->data->decoder_type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_SetLoop(selector->decoder, loop);
			break;

		case DECODER_TYPE_SIMPLE:
			selector->loop = loop;
			break;

		case DECODER_TYPE_COMPLEX:
			/* TODO - This is impossible to implement */
			break;
	}
}
