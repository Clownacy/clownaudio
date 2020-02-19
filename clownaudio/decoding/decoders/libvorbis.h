#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

Decoder* Decoder_libVorbis_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_libVorbis_Destroy(Decoder *decoder);
void Decoder_libVorbis_Rewind(Decoder *decoder);
size_t Decoder_libVorbis_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);
