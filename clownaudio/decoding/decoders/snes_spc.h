#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

Decoder* Decoder_SNES_SPC_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_SNES_SPC_Destroy(Decoder *decoder);
void Decoder_SNES_SPC_Rewind(Decoder *decoder);
size_t Decoder_SNES_SPC_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);
