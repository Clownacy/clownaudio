#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_SNES_SPC Decoder_SNES_SPC;

Decoder_SNES_SPC* Decoder_SNES_SPC_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_SNES_SPC_Destroy(Decoder_SNES_SPC *decoder);
void Decoder_SNES_SPC_Rewind(Decoder_SNES_SPC *decoder);
unsigned long Decoder_SNES_SPC_GetSamples(Decoder_SNES_SPC *decoder, void *buffer, unsigned long frames_to_do);
