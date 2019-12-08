#pragma once

#include <stddef.h>

#include "common.h"

typedef struct Decoder_Tremor Decoder_Tremor;

Decoder_Tremor* Decoder_Tremor_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_Tremor_Destroy(Decoder_Tremor *decoder);
void Decoder_Tremor_Rewind(Decoder_Tremor *decoder);
size_t Decoder_Tremor_GetSamples(Decoder_Tremor *decoder, void *buffer, size_t frames_to_do);
