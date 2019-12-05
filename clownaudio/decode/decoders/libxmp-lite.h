#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_libXMPLite Decoder_libXMPLite;

Decoder_libXMPLite* Decoder_libXMPLite_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_libXMPLite_Destroy(Decoder_libXMPLite *decoder);
void Decoder_libXMPLite_Rewind(Decoder_libXMPLite *decoder);
unsigned long Decoder_libXMPLite_GetSamples(Decoder_libXMPLite *decoder, void *buffer, unsigned long frames_to_do);
