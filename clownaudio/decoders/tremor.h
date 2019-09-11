#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_Tremor DecoderData_Tremor;
typedef struct Decoder_Tremor Decoder_Tremor;

DecoderData_Tremor* Decoder_Tremor_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_Tremor_UnloadData(DecoderData_Tremor *data);
Decoder_Tremor* Decoder_Tremor_Create(DecoderData_Tremor *data, bool loops, DecoderInfo *info);
void Decoder_Tremor_Destroy(Decoder_Tremor *this);
void Decoder_Tremor_Rewind(Decoder_Tremor *this);
unsigned long Decoder_Tremor_GetSamples(Decoder_Tremor *this, void *buffer, unsigned long bytes_to_do);
