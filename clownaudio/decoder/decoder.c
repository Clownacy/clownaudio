#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "backends/common.h"

#ifdef USE_LIBVORBIS
#include "backends/libvorbis.h"
#endif
#ifdef USE_TREMOR
#include "backends/tremor.h"
#endif
#ifdef USE_STB_VORBIS
#include "backends/stb_vorbis.h"
#endif
#ifdef USE_LIBFLAC
#include "backends/libflac.h"
#endif
#ifdef USE_DR_FLAC
#include "backends/dr_flac.h"
#endif
#ifdef USE_DR_WAV
#include "backends/dr_wav.h"
#endif
#ifdef USE_LIBSNDFILE
#include "backends/libsndfile.h"
#endif
#ifdef USE_LIBXMPLITE
#include "backends/libxmp-lite.h"
#endif
#ifdef USE_LIBOPENMPT
#include "backends/libopenmpt.h"
#endif
#ifdef USE_SNES_SPC
#include "backends/snes_spc.h"
#endif
#ifdef USE_PXTONE
#include "backends/pxtone.h"
#include "backends/pxtone_noise.h"
#endif

#define BACKEND_FUNCTIONS(name) \
{ \
	(void*(*)(DecoderData*,bool,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(unsigned long(*)(void*,void*,unsigned long))Decoder_##name##_GetSamples \
}

typedef struct DecoderBackendFunctions
{
	void* (*Create)(DecoderData *data, bool loops, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	unsigned long (*GetSamples)(void *decoder, void *buffer, unsigned long frames_to_do);
} DecoderBackendFunctions;

struct Decoder
{
	void *backend;
	const DecoderBackendFunctions *backend_functions;
};

static const DecoderBackendFunctions backend_functions[] = {
#ifdef USE_LIBVORBIS
	BACKEND_FUNCTIONS(libVorbis),
#endif
#ifdef USE_TREMOR
	BACKEND_FUNCTIONS(Tremor),
#endif
#ifdef USE_STB_VORBIS
	BACKEND_FUNCTIONS(STB_Vorbis),
#endif
#ifdef USE_LIBFLAC
	BACKEND_FUNCTIONS(libFLAC),
#endif
#ifdef USE_DR_FLAC
	BACKEND_FUNCTIONS(DR_FLAC),
#endif
#ifdef USE_DR_WAV
	BACKEND_FUNCTIONS(DR_WAV),
#endif
#ifdef USE_LIBSNDFILE
	BACKEND_FUNCTIONS(libSndfile),
#endif
#ifdef USE_LIBOPENMPT
	BACKEND_FUNCTIONS(libOpenMPT),
#endif
#ifdef USE_LIBXMPLITE
	BACKEND_FUNCTIONS(libXMPLite),
#endif
#ifdef USE_SNES_SPC
	BACKEND_FUNCTIONS(SNES_SPC),
#endif
#ifdef USE_PXTONE
	BACKEND_FUNCTIONS(PxTone),
	BACKEND_FUNCTIONS(PxToneNoise),
#endif
};

DecoderData* Decoder_LoadData(const unsigned char *file_buffer, size_t file_size)
{
	DecoderData *data = NULL;

	if (file_buffer != NULL)
	{
		data = malloc(sizeof(DecoderData));

		if (data != NULL)
		{
			data->file_buffer = file_buffer;
			data->file_size = file_size;
		}
	}

	return data;
}

void Decoder_UnloadData(DecoderData *data)
{
	free(data);
}

Decoder* Decoder_Create(DecoderData *data, bool loop, DecoderInfo *info)
{
	for (unsigned int i = 0; i < sizeof(backend_functions) / sizeof(backend_functions[0]); ++i)
	{
		void *backend = backend_functions[i].Create(data, loop, info);

		if (backend != NULL)
		{
			Decoder *decoder = malloc(sizeof(Decoder));

			if (decoder != NULL)
			{
				decoder->backend = backend;
				decoder->backend_functions = &backend_functions[i];
				return decoder;
			}

			backend_functions[i].Destroy(backend);
		}
	}

	return NULL;
}

void Decoder_Destroy(Decoder *decoder)
{
	if (decoder != NULL)
	{
		decoder->backend_functions->Destroy(decoder->backend);
		free(decoder);
	}
}

void Decoder_Rewind(Decoder *decoder)
{
	decoder->backend_functions->Rewind(decoder->backend);
}

unsigned long Decoder_GetSamples(Decoder *decoder, void *buffer, unsigned long frames_to_do)
{
	return decoder->backend_functions->GetSamples(decoder->backend, buffer, frames_to_do);
}
