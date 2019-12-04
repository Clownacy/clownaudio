#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_STB_Vorbis Decoder_STB_Vorbis;

Decoder_STB_Vorbis* Decoder_STB_Vorbis_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_STB_Vorbis_Destroy(Decoder_STB_Vorbis *decoder);
void Decoder_STB_Vorbis_Rewind(Decoder_STB_Vorbis *decoder);
size_t Decoder_STB_Vorbis_GetSamples(Decoder_STB_Vorbis *decoder, void *buffer, size_t frames_to_do);
