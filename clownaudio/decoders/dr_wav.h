// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_DR_WAV DecoderData_DR_WAV;
typedef struct Decoder_DR_WAV Decoder_DR_WAV;

DecoderData_DR_WAV* Decoder_DR_WAV_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_DR_WAV_UnloadData(DecoderData_DR_WAV *data);
Decoder_DR_WAV* Decoder_DR_WAV_Create(DecoderData_DR_WAV *data, bool loops, DecoderInfo *info);
void Decoder_DR_WAV_Destroy(Decoder_DR_WAV *this);
void Decoder_DR_WAV_Rewind(Decoder_DR_WAV *this);
unsigned long Decoder_DR_WAV_GetSamples(Decoder_DR_WAV *this, void *buffer_void, unsigned long frames_to_do);
