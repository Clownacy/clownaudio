#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_libFLAC Decoder_libFLAC;

Decoder_libFLAC* Decoder_libFLAC_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_libFLAC_Destroy(Decoder_libFLAC *decoder);
void Decoder_libFLAC_Rewind(Decoder_libFLAC *decoder);
unsigned long Decoder_libFLAC_GetSamples(Decoder_libFLAC *decoder, void *buffer, unsigned long frames_to_do);
