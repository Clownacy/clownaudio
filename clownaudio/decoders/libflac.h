// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_libFLAC DecoderData_libFLAC;
typedef struct Decoder_libFLAC Decoder_libFLAC;

DecoderData_libFLAC* Decoder_libFLAC_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_libFLAC_UnloadData(DecoderData_libFLAC *data);
Decoder_libFLAC* Decoder_libFLAC_Create(DecoderData_libFLAC *data, bool loops, DecoderInfo *info);
void Decoder_libFLAC_Destroy(Decoder_libFLAC *this);
void Decoder_libFLAC_Rewind(Decoder_libFLAC *this);
unsigned long Decoder_libFLAC_GetSamples(Decoder_libFLAC *this, void *buffer, unsigned long bytes_to_do);
