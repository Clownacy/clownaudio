#include "high-level-decoder_selector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoders/common.h"

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
#endif

#define DECODER_FUNCTIONS(name) \
{ \
	(void*(*)(const unsigned char*,size_t,bool,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(size_t(*)(void*,void*,size_t))Decoder_##name##_GetSamples \
}

typedef struct HighLevelDecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} HighLevelDecoderFunctions;

struct HighLevelDecoderSelector
{
	void *decoder;
	const HighLevelDecoderFunctions *decoder_functions;
};

static const HighLevelDecoderFunctions decoder_functions[] = {
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
};

HighLevelDecoderSelector* HighLevelDecoderSelector_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	if (data != NULL)
	{
		for (unsigned int i = 0; i < sizeof(decoder_functions) / sizeof(decoder_functions[0]); ++i)
		{
			void *decoder = decoder_functions[i].Create(data, data_size, loop, info);

			if (decoder != NULL)
			{
				HighLevelDecoderSelector *selector = malloc(sizeof(HighLevelDecoderSelector));

				if (selector != NULL)
				{
					selector->decoder = decoder;
					selector->decoder_functions = &decoder_functions[i];

					return selector;
				}

				decoder_functions[i].Destroy(decoder);
			}
		}
	}

	return NULL;
}

void HighLevelDecoderSelector_Destroy(HighLevelDecoderSelector *selector)
{
	selector->decoder_functions->Destroy(selector->decoder);
	free(selector);
}

void HighLevelDecoderSelector_Rewind(HighLevelDecoderSelector *selector)
{
	selector->decoder_functions->Rewind(selector->decoder);
}

size_t HighLevelDecoderSelector_GetSamples(HighLevelDecoderSelector *selector, void *buffer, size_t frames_to_do)
{
	return selector->decoder_functions->GetSamples(selector->decoder, buffer, frames_to_do);
}
