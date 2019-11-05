#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_libXMPLite Decoder_libXMPLite;

Decoder_libXMPLite* Decoder_libXMPLite_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_libXMPLite_Destroy(Decoder_libXMPLite *decoder);
void Decoder_libXMPLite_Rewind(Decoder_libXMPLite *decoder);
unsigned long Decoder_libXMPLite_GetSamples(Decoder_libXMPLite *decoder, void *buffer, unsigned long frames_to_do);
