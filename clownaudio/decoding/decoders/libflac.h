#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_libFLAC Decoder_libFLAC;

Decoder_libFLAC* Decoder_libFLAC_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_libFLAC_Destroy(Decoder_libFLAC *decoder);
void Decoder_libFLAC_Rewind(Decoder_libFLAC *decoder);
size_t Decoder_libFLAC_GetSamples(Decoder_libFLAC *decoder, void *buffer, size_t frames_to_do);
