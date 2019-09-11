#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_Pxtone DecoderData_Pxtone;
typedef struct Decoder_Pxtone Decoder_Pxtone;

DecoderData_Pxtone* Decoder_Pxtone_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_Pxtone_UnloadData(DecoderData_Pxtone *data);
Decoder_Pxtone* Decoder_Pxtone_Create(DecoderData_Pxtone *data, bool loops, DecoderInfo *info);
void Decoder_Pxtone_Destroy(Decoder_Pxtone *decoder);
void Decoder_Pxtone_Rewind(Decoder_Pxtone *decoder);
unsigned long Decoder_Pxtone_GetSamples(Decoder_Pxtone *decoder, void *buffer, unsigned long frames_to_do);
