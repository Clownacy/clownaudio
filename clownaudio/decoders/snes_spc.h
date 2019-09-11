#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_SNES_SPC DecoderData_SNES_SPC;
typedef struct Decoder_SNES_SPC Decoder_SNES_SPC;

DecoderData_SNES_SPC* Decoder_SNES_SPC_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_SNES_SPC_UnloadData(DecoderData_SNES_SPC *data);
Decoder_SNES_SPC* Decoder_SNES_SPC_Create(DecoderData_SNES_SPC *data, bool loops, DecoderInfo *info);
void Decoder_SNES_SPC_Destroy(Decoder_SNES_SPC *this);
void Decoder_SNES_SPC_Rewind(Decoder_SNES_SPC *this);
unsigned long Decoder_SNES_SPC_GetSamples(Decoder_SNES_SPC *this, void *buffer, unsigned long bytes_to_do);
