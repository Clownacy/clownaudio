#include "decoder_selector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

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

#define LOW_LEVEL_DECODER_FUNCTIONS(name) \
{ \
	(void*(*)(const unsigned char*,size_t,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(size_t(*)(void*,void*,size_t))Decoder_##name##_GetSamples \
}

#define HIGH_LEVEL_DECODER_FUNCTIONS(name) \
{ \
	(void*(*)(const unsigned char*,size_t,bool,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(size_t(*)(void*,void*,size_t))Decoder_##name##_GetSamples \
}

typedef enum DecoderType
{
	DECODER_TYPE_PREDECODER,
	DECODER_TYPE_HIGH_LEVEL,
	DECODER_TYPE_LOW_LEVEL
} DecoderType;

struct DecoderSelectorData
{
	const unsigned char *file_buffer;
	size_t file_size;
	DecoderType decoder_type;
	const void *decoder_functions;
	PredecoderData *predecoder_data;
	size_t size_of_frame;
};

struct DecoderSelector
{
	void *decoder;
	DecoderSelectorData *data;
	bool loop;
};

typedef struct LowLevelDecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} LowLevelDecoderFunctions;

typedef struct HighLevelDecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} HighLevelDecoderFunctions;

static const LowLevelDecoderFunctions low_level_decoder_functions[] = {
#ifdef USE_LIBVORBIS
	LOW_LEVEL_DECODER_FUNCTIONS(libVorbis),
#endif
#ifdef USE_STB_VORBIS
	LOW_LEVEL_DECODER_FUNCTIONS(STB_Vorbis),
#endif
#ifdef USE_LIBFLAC
	LOW_LEVEL_DECODER_FUNCTIONS(libFLAC),
#endif
#ifdef USE_DR_FLAC
	LOW_LEVEL_DECODER_FUNCTIONS(DR_FLAC),
#endif
#ifdef USE_DR_WAV
	LOW_LEVEL_DECODER_FUNCTIONS(DR_WAV),
#endif
#ifdef USE_LIBOPUS
	LOW_LEVEL_DECODER_FUNCTIONS(libOpus),
#endif
#ifdef USE_LIBSNDFILE
	LOW_LEVEL_DECODER_FUNCTIONS(libSndfile),
#endif
#ifdef USE_PXTONE
	LOW_LEVEL_DECODER_FUNCTIONS(PxToneNoise),
#endif
};

static const HighLevelDecoderFunctions high_level_decoder_functions[] = {
#ifdef USE_LIBOPENMPT
	HIGH_LEVEL_DECODER_FUNCTIONS(libOpenMPT),
#endif
#ifdef USE_LIBXMPLITE
	HIGH_LEVEL_DECODER_FUNCTIONS(libXMPLite),
#endif
#ifdef USE_SNES_SPC
	HIGH_LEVEL_DECODER_FUNCTIONS(SNES_SPC),
#endif
#ifdef USE_PXTONE
	HIGH_LEVEL_DECODER_FUNCTIONS(PxTone),
#endif
};

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode)
{
	DecoderType decoder_type;
	const void *decoder_functions = NULL;
	PredecoderData *predecoder_data = NULL;

	DecoderInfo info;

	// Figure out what format this sound is

	// Start with the low-level decoders
	for (size_t i = 0; i < sizeof(low_level_decoder_functions) / sizeof(low_level_decoder_functions[0]); ++i)
	{
		void *decoder = low_level_decoder_functions[i].Create(file_buffer, file_size, &info);

		if (decoder != NULL)
		{
			decoder_type = DECODER_TYPE_LOW_LEVEL;
			decoder_functions = &low_level_decoder_functions[i];

			if (predecode)
			{
				predecoder_data = Predecoder_DecodeData(&info, decoder, low_level_decoder_functions[i].GetSamples);

				if (predecoder_data != NULL)
					decoder_type = DECODER_TYPE_PREDECODER;
			}

			low_level_decoder_functions[i].Destroy(decoder);

			break;
		}
	}

	// Failing that, search the high-level formats instead
	if (decoder_functions == NULL)
	{
		for (size_t i = 0; i < sizeof(high_level_decoder_functions) / sizeof(high_level_decoder_functions[0]); ++i)
		{
			void *decoder = high_level_decoder_functions[i].Create(file_buffer, file_size, false, &info);

			if (decoder != NULL)
			{
				high_level_decoder_functions[i].Destroy(decoder);

				decoder_type = DECODER_TYPE_HIGH_LEVEL;
				decoder_functions = &high_level_decoder_functions[i];
				break;
			}
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

DecoderSelector* DecoderSelector_Create(DecoderSelectorData *data, bool loop, DecoderInfo *info)
{
	DecoderSelector *selector = malloc(sizeof(DecoderSelector));

	if (selector != NULL)
	{
		switch (data->decoder_type)
		{
			case DECODER_TYPE_PREDECODER:
				selector->decoder = Predecoder_Create(data->predecoder_data, loop, info);
				break;

			case DECODER_TYPE_HIGH_LEVEL:
				selector->decoder = ((HighLevelDecoderFunctions*)data->decoder_functions)->Create(data->file_buffer, data->file_size, loop, info);
				break;

			case DECODER_TYPE_LOW_LEVEL:
				selector->decoder = ((LowLevelDecoderFunctions*)data->decoder_functions)->Create(data->file_buffer, data->file_size, info);
				break;
		}

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
	switch (selector->data->decoder_type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_Destroy(selector->decoder);
			break;

		case DECODER_TYPE_HIGH_LEVEL:
			((HighLevelDecoderFunctions*)selector->data->decoder_functions)->Destroy(selector->decoder);
			break;

		case DECODER_TYPE_LOW_LEVEL:
			((LowLevelDecoderFunctions*)selector->data->decoder_functions)->Destroy(selector->decoder);
			break;
	}

	free(selector);
}

void DecoderSelector_Rewind(DecoderSelector *selector)
{
	switch (selector->data->decoder_type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_Rewind(selector->decoder);
			break;

		case DECODER_TYPE_HIGH_LEVEL:
			((HighLevelDecoderFunctions*)selector->data->decoder_functions)->Rewind(selector->decoder);
			break;

		case DECODER_TYPE_LOW_LEVEL:
			((LowLevelDecoderFunctions*)selector->data->decoder_functions)->Rewind(selector->decoder);
			break;
	}
}

size_t DecoderSelector_GetSamples(DecoderSelector *selector, void *buffer, size_t frames_to_do)
{
	if (selector->data->decoder_type == DECODER_TYPE_PREDECODER)
	{
		return Predecoder_GetSamples(selector->decoder, buffer, frames_to_do);
	}
	else if (selector->data->decoder_type == DECODER_TYPE_HIGH_LEVEL)
	{
		return ((HighLevelDecoderFunctions*)selector->data->decoder_functions)->GetSamples(selector->decoder, buffer, frames_to_do);
	}
	else //if (selector->data->decoder_type == DECODER_TYPE_LOW_LEVEL)
	{
		// Handle looping here, since the low-level decoders don't do it by themselves
		size_t frames_done = 0;

		while (frames_done != frames_to_do)
		{
			const size_t frames = ((LowLevelDecoderFunctions*)selector->data->decoder_functions)->GetSamples(selector->decoder, &((char*)buffer)[frames_done * selector->data->size_of_frame], frames_to_do - frames_done);

			if (frames == 0)
			{
				if (selector->loop)
					((LowLevelDecoderFunctions*)selector->data->decoder_functions)->Rewind(selector->decoder);
				else
					break;
			}

			frames_done += frames;
		}

		return frames_done;
	}
}
