#include "low-level-decoder_selector.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "decoders/common.h"

#ifdef USE_LIBVORBIS
#include "decoders/libvorbis.h"
#endif
#ifdef USE_TREMOR
#include "decoders/tremor.h"
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
#ifdef USE_LIBSNDFILE
#include "decoders/libsndfile.h"
#endif
#ifdef USE_PXTONE
#include "decoders/pxtone_noise.h"
#endif

#define DECODER_FUNCTIONS(name) \
{ \
	(void*(*)(const unsigned char*,size_t,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(size_t(*)(void*,void*,size_t))Decoder_##name##_GetSamples \
}

typedef struct LowLevelDecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} LowLevelDecoderFunctions;

struct LowLevelDecoderSelector
{
	void *decoder;
	const LowLevelDecoderFunctions *decoder_functions;
	bool loop;
	size_t size_of_frame;
};

static const LowLevelDecoderFunctions decoder_functions[] = {
#ifdef USE_LIBVORBIS
	DECODER_FUNCTIONS(libVorbis),
#endif
#ifdef USE_TREMOR
	DECODER_FUNCTIONS(Tremor),
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
#ifdef USE_LIBSNDFILE
	DECODER_FUNCTIONS(libSndfile),
#endif
#ifdef USE_PXTONE
	DECODER_FUNCTIONS(PxToneNoise),
#endif
};

LowLevelDecoderSelector* LowLevelDecoderSelector_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info)
{
	if (data != NULL)
	{
		for (unsigned int i = 0; i < sizeof(decoder_functions) / sizeof(decoder_functions[0]); ++i)
		{
			void *decoder = decoder_functions[i].Create(data, data_size, info);

			if (decoder != NULL)
			{
				LowLevelDecoderSelector *selector = malloc(sizeof(LowLevelDecoderSelector));

				if (selector != NULL)
				{
					selector->decoder = decoder;
					selector->decoder_functions = &decoder_functions[i];
					selector->loop = loop;
					selector->size_of_frame = info->channel_count;

					switch (info->format)
					{
						case DECODER_FORMAT_S16:
							selector->size_of_frame *= 2;
							break;

						case DECODER_FORMAT_S32:
						case DECODER_FORMAT_F32:
							selector->size_of_frame *= 4;
							break;
					}

					return selector;
				}

				decoder_functions[i].Destroy(decoder);
			}
		}
	}

	return NULL;
}

void LowLevelDecoderSelector_Destroy(LowLevelDecoderSelector *selector)
{
	selector->decoder_functions->Destroy(selector->decoder);
	free(selector);
}

void LowLevelDecoderSelector_Rewind(LowLevelDecoderSelector *selector)
{
	selector->decoder_functions->Rewind(selector->decoder);
}

size_t LowLevelDecoderSelector_GetSamples(LowLevelDecoderSelector *selector, void *buffer, size_t frames_to_do)
{
	size_t frames_done = 0;

	while (frames_done != frames_to_do)
	{
		const size_t frames = selector->decoder_functions->GetSamples(selector->decoder, &((char*)buffer)[frames_done * selector->size_of_frame], frames_to_do - frames_done);

		if (frames == 0)
		{
			if (selector->loop)
				selector->decoder_functions->Rewind(selector->decoder);
			else
				break;
		}

		frames_done += frames;
	}

	return frames_done;
}
