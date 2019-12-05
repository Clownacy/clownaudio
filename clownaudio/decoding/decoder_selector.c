#include "decoder_selector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoders/common.h"
#include "high-level-decoder_selector.h"
#include "low-level-decoder_selector.h"
#include "predecoder.h"

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
	PredecoderData *predecoder_data;
};

struct DecoderSelector
{
	void *decoder;
	DecoderType type;
};

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode)
{
	DecoderSelectorData *data = malloc(sizeof(DecoderSelectorData));

	if (data != NULL)
	{
		data->file_buffer = file_buffer;
		data->file_size = file_size;
		data->predecoder_data = predecode ? Predecoder_DecodeData(file_buffer, file_size) : NULL;
	}

	return data;
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
		selector->decoder = NULL;

		if (data->predecoder_data != NULL)
			selector->decoder = Predecoder_Create(data->predecoder_data, loop, info);

		if (selector->decoder != NULL)
		{
			selector->type = DECODER_TYPE_PREDECODER;
			return selector;
		}

		selector->decoder = HighLevelDecoderSelector_Create(data->file_buffer, data->file_size, loop, info);

		if (selector->decoder != NULL)
		{
			selector->type = DECODER_TYPE_HIGH_LEVEL;
			return selector;
		}

		selector->decoder = LowLevelDecoderSelector_Create(data->file_buffer, data->file_size, loop, info);

		if (selector->decoder != NULL)
		{
			selector->type = DECODER_TYPE_LOW_LEVEL;
			return selector;
		}

		free(selector);
	}

	return NULL;
}

void DecoderSelector_Destroy(DecoderSelector *selector)
{
	switch (selector->type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_Destroy(selector->decoder);
			break;

		case DECODER_TYPE_HIGH_LEVEL:
			HighLevelDecoderSelector_Destroy(selector->decoder);
			break;

		case DECODER_TYPE_LOW_LEVEL:
			LowLevelDecoderSelector_Destroy(selector->decoder);
			break;
	}

	free(selector);
}

void DecoderSelector_Rewind(DecoderSelector *selector)
{
	switch (selector->type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_Rewind(selector->decoder);
			break;

		case DECODER_TYPE_HIGH_LEVEL:
			HighLevelDecoderSelector_Rewind(selector->decoder);
			break;

		case DECODER_TYPE_LOW_LEVEL:
			LowLevelDecoderSelector_Rewind(selector->decoder);
			break;
	}
}

size_t DecoderSelector_GetSamples(DecoderSelector *selector, void *buffer, size_t frames_to_do)
{
	switch (selector->type)
	{
		case DECODER_TYPE_PREDECODER:
			return Predecoder_GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_HIGH_LEVEL:
			return HighLevelDecoderSelector_GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_LOW_LEVEL:
			return LowLevelDecoderSelector_GetSamples(selector->decoder, buffer, frames_to_do);

		default:
			return 1;	// Stupid compiler warnings. We *totally* have to account for memory-corruption, am I right?
	}
}
