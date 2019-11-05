#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_STB_Vorbis Decoder_STB_Vorbis;

Decoder_STB_Vorbis* Decoder_STB_Vorbis_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_STB_Vorbis_Destroy(Decoder_STB_Vorbis *decoder);
void Decoder_STB_Vorbis_Rewind(Decoder_STB_Vorbis *decoder);
unsigned long Decoder_STB_Vorbis_GetSamples(Decoder_STB_Vorbis *decoder, void *buffer_void, unsigned long frames_to_do);
