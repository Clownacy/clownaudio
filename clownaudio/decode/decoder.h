#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "backends/common.h"

typedef struct Decoder Decoder;

DecoderData* Decoder_LoadData(const unsigned char *file_buffer, size_t file_size);
void Decoder_UnloadData(DecoderData *data);
Decoder* Decoder_Create(DecoderData *data, bool loop, DecoderInfo *info);
void Decoder_Destroy(Decoder *decoder);
void Decoder_Rewind(Decoder *decoder);
unsigned long Decoder_GetSamples(Decoder *decoder, void *buffer, unsigned long frames_to_do);
