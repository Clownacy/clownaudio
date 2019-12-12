#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "decoders/common.h"

typedef struct PredecoderData PredecoderData;
typedef struct Predecoder Predecoder;

PredecoderData* Predecoder_DecodeData(const unsigned char *data, size_t data_size, const LowLevelDecoderFunctions *decoder_functions);
void Predecoder_UnloadData(PredecoderData *data);
Predecoder* Predecoder_Create(PredecoderData *data, bool loop, DecoderInfo *info);
void Predecoder_Destroy(Predecoder *predecoder);
void Predecoder_Rewind(Predecoder *predecoder);
size_t Predecoder_GetSamples(Predecoder *predecoder, void *buffer, size_t frames_to_do);
