#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

Decoder* Decoder_DR_FLAC_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_DR_FLAC_Destroy(Decoder *decoder);
void Decoder_DR_FLAC_Rewind(Decoder *decoder);
size_t Decoder_DR_FLAC_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);
