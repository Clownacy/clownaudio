#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Decoder_PxToneNoise Decoder_PxToneNoise;

Decoder_PxToneNoise* Decoder_PxToneNoise_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_PxToneNoise_Destroy(Decoder_PxToneNoise *decoder);
void Decoder_PxToneNoise_Rewind(Decoder_PxToneNoise *decoder);
size_t Decoder_PxToneNoise_GetSamples(Decoder_PxToneNoise *decoder, void *buffer, size_t frames_to_do);

#ifdef __cplusplus
}
#endif
