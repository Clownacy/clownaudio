#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_PxTone Decoder_PxTone;

Decoder_PxTone* Decoder_PxTone_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_PxTone_Destroy(Decoder_PxTone *decoder);
void Decoder_PxTone_Rewind(Decoder_PxTone *decoder);
unsigned long Decoder_PxTone_GetSamples(Decoder_PxTone *decoder, void *buffer, unsigned long frames_to_do);
