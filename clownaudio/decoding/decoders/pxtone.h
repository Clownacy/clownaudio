#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Decoder_PxTone Decoder_PxTone;

Decoder_PxTone* Decoder_PxTone_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_PxTone_Destroy(Decoder_PxTone *decoder);
void Decoder_PxTone_Rewind(Decoder_PxTone *decoder);
unsigned long Decoder_PxTone_GetSamples(Decoder_PxTone *decoder, void *buffer, unsigned long frames_to_do);

#ifdef __cplusplus
}
#endif
