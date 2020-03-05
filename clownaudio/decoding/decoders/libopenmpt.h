#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_libOpenMPT Decoder_libOpenMPT;

Decoder_libOpenMPT* Decoder_libOpenMPT_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_libOpenMPT_Destroy(Decoder_libOpenMPT *decoder);
void Decoder_libOpenMPT_Rewind(Decoder_libOpenMPT *decoder);
size_t Decoder_libOpenMPT_GetSamples(Decoder_libOpenMPT *decoder, void *buffer, size_t frames_to_do);
