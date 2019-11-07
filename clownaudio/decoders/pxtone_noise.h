#pragma once

#include <stdbool.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Decoder_PxToneNoise Decoder_PxToneNoise;

Decoder_PxToneNoise* Decoder_PxToneNoise_Create(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_PxToneNoise_Destroy(Decoder_PxToneNoise *decoder);
void Decoder_PxToneNoise_Rewind(Decoder_PxToneNoise *decoder);
unsigned long Decoder_PxToneNoise_GetSamples(Decoder_PxToneNoise *decoder, void *buffer, unsigned long frames_to_do);

#ifdef __cplusplus
}
#endif
