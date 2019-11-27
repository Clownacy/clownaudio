#include "decoder.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "miniaudio.h"

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

#define BACKEND_FUNCTIONS(name) \
{ \
	(void*(*)(DecoderData*,bool,DecoderInfo*))Decoder_##name##_Create, \
	(void(*)(void*))Decoder_##name##_Destroy, \
	(void(*)(void*))Decoder_##name##_Rewind, \
	(unsigned long(*)(void*,void*,unsigned long))Decoder_##name##_GetSamples \
}

typedef struct DecoderBackend
{
	void* (*Create)(DecoderData *data, bool loops, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	unsigned long (*GetSamples)(void *decoder, void *buffer, unsigned long frames_to_do);
} DecoderBackend;

struct Decoder
{
	void *backend;
	const DecoderBackend *backend_functions;
	ma_pcm_converter converter;
};

static const DecoderBackend backends[] = {
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

static ma_uint32 PCMConverterCallback(ma_pcm_converter *converter, void *output_buffer, ma_uint32 frames_to_do, void *user_data)
{
	(void)converter;

	Decoder *decoder = (Decoder*)user_data;

	return decoder->backend_functions->GetSamples(decoder->backend, output_buffer, frames_to_do);
}

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
	//if (data != NULL)
	//{
	//	free(data->file_buffer);
		free(data);
	//}
}

Decoder* Decoder_Create(DecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count)
{
	for (unsigned int i = 0; i < sizeof(backends) / sizeof(backends[0]); ++i)
	{
		DecoderInfo info;
		void *backend = backends[i].Create(data, loop, &info);

		if (backend != NULL)
		{
			Decoder *decoder = malloc(sizeof(Decoder));

			if (decoder != NULL)
			{
				decoder->backend = backend;
				decoder->backend_functions = &backends[i];

				ma_format format;
				switch (info.format)
				{
					case DECODER_FORMAT_S16:
						format = ma_format_s16;
						break;

					case DECODER_FORMAT_S32:
						format = ma_format_s32;
						break;

					case DECODER_FORMAT_F32:
						format = ma_format_f32;
						break;

					default:
						backends[i].Destroy(backend);
						free(decoder);
						return NULL;
				}

				const ma_pcm_converter_config config = ma_pcm_converter_config_init(format, info.channel_count, info.sample_rate, ma_format_f32, channel_count, sample_rate, PCMConverterCallback, decoder);
				ma_pcm_converter_init(&config, &decoder->converter);

				return decoder;
			}
			else
			{
				backends[i].Destroy(backend);
			}
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
	return (unsigned long)ma_pcm_converter_read(&decoder->converter, buffer, frames_to_do);
}
