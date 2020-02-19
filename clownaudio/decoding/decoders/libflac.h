#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

Decoder* Decoder_libFLAC_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_libFLAC_Destroy(Decoder *decoder);
void Decoder_libFLAC_Rewind(Decoder *decoder);
size_t Decoder_libFLAC_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);
