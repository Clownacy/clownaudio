#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

Decoder* Decoder_PxTone_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_PxTone_Destroy(Decoder *decoder);
void Decoder_PxTone_Rewind(Decoder *decoder);
size_t Decoder_PxTone_GetSamples(Decoder *decoder, void *buffer, size_t frames_to_do);

#ifdef __cplusplus
}
#endif
