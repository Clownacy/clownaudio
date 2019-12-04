#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_Tremor Decoder_Tremor;

Decoder_Tremor* Decoder_Tremor_Create(DecoderData *data, bool loop, DecoderInfo *info);
void Decoder_Tremor_Destroy(Decoder_Tremor *decoder);
void Decoder_Tremor_Rewind(Decoder_Tremor *decoder);
unsigned long Decoder_Tremor_GetSamples(Decoder_Tremor *decoder, void *buffer, unsigned long frames_to_do);
