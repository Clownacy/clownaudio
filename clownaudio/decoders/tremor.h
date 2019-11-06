#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_Tremor Decoder_Tremor;

Decoder_Tremor* Decoder_Tremor_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_Tremor_Destroy(Decoder_Tremor *decoder);
void Decoder_Tremor_Rewind(Decoder_Tremor *decoder);
unsigned long Decoder_Tremor_GetSamples(Decoder_Tremor *decoder, void *buffer, unsigned long frames_to_do);
