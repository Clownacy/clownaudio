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

typedef enum DecoderType
{
	DECODER_TYPE_PREDECODER,
	DECODER_TYPE_COMPLEX,
	DECODER_TYPE_SIMPLE
} DecoderType;

typedef struct DecoderFunctions
{
	Decoder* (*Create)(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
	void (*Destroy)(Decoder *decoder);
	void (*Rewind)(Decoder *decoder);
	size_t (*GetSamples)(Decoder *decoder, void *buffer, size_t frames_to_do);
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
	Decoder *decoder;
	DecoderSelectorData *data;
	bool loop;
};

static const DecoderFunctions decoder_function_list[] = {
#ifdef USE_LIBVORBIS
	{
		Decoder_libVorbis_Create,
		Decoder_libVorbis_Destroy,
		Decoder_libVorbis_Rewind,
		Decoder_libVorbis_GetSamples,
	},
#endif
#ifdef USE_STB_VORBIS
	{
		Decoder_STB_Vorbis_Create,
		Decoder_STB_Vorbis_Destroy,
		Decoder_STB_Vorbis_Rewind,
		Decoder_STB_Vorbis_GetSamples,
	},
#endif
#ifdef USE_LIBFLAC
	{
		Decoder_libFLAC_Create,
		Decoder_libFLAC_Destroy,
		Decoder_libFLAC_Rewind,
		Decoder_libFLAC_GetSamples,
	},
#endif
#ifdef USE_DR_FLAC
	{
		Decoder_DR_FLAC_Create,
		Decoder_DR_FLAC_Destroy,
		Decoder_DR_FLAC_Rewind,
		Decoder_DR_FLAC_GetSamples,
	},
#endif
#ifdef USE_DR_WAV
	{
		Decoder_DR_WAV_Create,
		Decoder_DR_WAV_Destroy,
		Decoder_DR_WAV_Rewind,
		Decoder_DR_WAV_GetSamples,
	},
#endif
#ifdef USE_LIBOPUS
	{
		Decoder_libOpus_Create,
		Decoder_libOpus_Destroy,
		Decoder_libOpus_Rewind,
		Decoder_libOpus_GetSamples,
	},
#endif
#ifdef USE_LIBSNDFILE
	{
		Decoder_libSndfile_Create,
		Decoder_libSndfile_Destroy,
		Decoder_libSndfile_Rewind,
		Decoder_libSndfile_GetSamples,
	},
#endif
#ifdef USE_LIBOPENMPT
	{
		Decoder_libOpenMPT_Create,
		Decoder_libOpenMPT_Destroy,
		Decoder_libOpenMPT_Rewind,
		Decoder_libOpenMPT_GetSamples,
	},
#endif
#ifdef USE_LIBXMPLITE
	{
		Decoder_libXMPLite_Create,
		Decoder_libXMPLite_Destroy,
		Decoder_libXMPLite_Rewind,
		Decoder_libXMPLite_GetSamples,
	},
#endif
#ifdef USE_SNES_SPC
	{
		Decoder_SNES_SPC_Create,
		Decoder_SNES_SPC_Destroy,
		Decoder_SNES_SPC_Rewind,
		Decoder_SNES_SPC_GetSamples,
	},
#endif
#ifdef USE_PXTONE
	{
		Decoder_PxTone_Create,
		Decoder_PxTone_Destroy,
		Decoder_PxTone_Rewind,
		Decoder_PxTone_GetSamples,
	},
#endif
#ifdef USE_PXTONE
	{
		Decoder_PxToneNoise_Create,
		Decoder_PxToneNoise_Destroy,
		Decoder_PxToneNoise_Rewind,
		Decoder_PxToneNoise_GetSamples,
	},
#endif
};

static const DecoderFunctions predecoder_functions = {
	NULL,
	(void(*)(Decoder*))Predecoder_Destroy,
	(void(*)(Decoder*))Predecoder_Rewind,
	(size_t(*)(Decoder*,void*,size_t))Predecoder_GetSamples
};

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode)
{
	DecoderType decoder_type;
	const DecoderFunctions *decoder_functions = NULL;
	PredecoderData *predecoder_data = NULL;

	DecoderInfo info;

	// Figure out what format this sound is
	for (size_t i = 0; i < sizeof(decoder_function_list) / sizeof(decoder_function_list[0]); ++i)
	{
		Decoder *decoder = decoder_function_list[i].Create(file_buffer, file_size, false, &info);

		if (decoder != NULL)
		{
			decoder_type = info.complex ? DECODER_TYPE_COMPLEX : DECODER_TYPE_SIMPLE;
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

DecoderSelector* DecoderSelector_Create(DecoderSelectorData *data, bool loop, DecoderInfo *info)
{
	DecoderSelector *selector = malloc(sizeof(DecoderSelector));

	if (selector != NULL)
	{
		if (data->decoder_type == DECODER_TYPE_PREDECODER)
			selector->decoder = (Decoder*)Predecoder_Create(data->predecoder_data, loop, info);
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
	switch (selector->data->decoder_type)
	{
		case DECODER_TYPE_PREDECODER:
		case DECODER_TYPE_COMPLEX:
			return selector->data->decoder_functions->GetSamples(selector->decoder, buffer, frames_to_do);

		case DECODER_TYPE_SIMPLE:;
			// Handle looping here, since the simple decoders don't do it by themselves
			size_t frames_done = 0;

			while (frames_done != frames_to_do)
			{
				const size_t frames = selector->data->decoder_functions->GetSamples(selector->decoder, &((char*)buffer)[frames_done * selector->data->size_of_frame], frames_to_do - frames_done);

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

	// In case decoder_type is somehow invalid
	return frames_to_do;
}

void DecoderSelector_SetLoop(DecoderSelector *selector, bool loop)
{
	switch (selector->data->decoder_type)
	{
		case DECODER_TYPE_PREDECODER:
			Predecoder_SetLoop((Predecoder*)selector->decoder, loop);
			break;

		case DECODER_TYPE_SIMPLE:
			selector->loop = loop;
			break;

		case DECODER_TYPE_COMPLEX:
			// TODO - This is impossible to implement
			break;
	}
}
