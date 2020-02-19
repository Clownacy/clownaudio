#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

Decoder* Decoder_libXMPLite_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_libXMPLite_Destroy(Decoder *decoder);
void Decoder_libXMPLite_Rewind(Decoder *decoder);
size_t Decoder_libXMPLite_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);
