#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_libOpenMPT Decoder_libOpenMPT;

Decoder_libOpenMPT* Decoder_libOpenMPT_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_libOpenMPT_Destroy(Decoder_libOpenMPT *decoder);
void Decoder_libOpenMPT_Rewind(Decoder_libOpenMPT *decoder);
unsigned long Decoder_libOpenMPT_GetSamples(Decoder_libOpenMPT *decoder, void *buffer, unsigned long frames_to_do);
