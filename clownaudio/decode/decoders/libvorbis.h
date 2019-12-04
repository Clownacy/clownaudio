#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "../common.h"

typedef struct Decoder_libVorbis Decoder_libVorbis;

Decoder_libVorbis* Decoder_libVorbis_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_libVorbis_Destroy(Decoder_libVorbis *decoder);
void Decoder_libVorbis_Rewind(Decoder_libVorbis *decoder);
size_t Decoder_libVorbis_GetSamples(Decoder_libVorbis *decoder, void *buffer, size_t frames_to_do);
