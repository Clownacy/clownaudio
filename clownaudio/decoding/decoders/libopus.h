#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

Decoder* Decoder_libOpus_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_libOpus_Destroy(Decoder *decoder);
void Decoder_libOpus_Rewind(Decoder *decoder);
size_t Decoder_libOpus_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);
