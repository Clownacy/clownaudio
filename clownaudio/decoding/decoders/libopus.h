#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_libOpus Decoder_libOpus;

Decoder_libOpus* Decoder_libOpus_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_libOpus_Destroy(Decoder_libOpus *decoder);
void Decoder_libOpus_Rewind(Decoder_libOpus *decoder);
size_t Decoder_libOpus_GetSamples(Decoder_libOpus *decoder, void *buffer, size_t frames_to_do);
