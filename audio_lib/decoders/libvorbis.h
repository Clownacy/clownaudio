// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_libVorbis DecoderData_libVorbis;
typedef struct Decoder_libVorbis Decoder_libVorbis;

DecoderData_libVorbis* Decoder_libVorbis_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_libVorbis_UnloadData(DecoderData_libVorbis *data);
Decoder_libVorbis* Decoder_libVorbis_Create(DecoderData_libVorbis *data, bool loops, DecoderInfo *info);
void Decoder_libVorbis_Destroy(Decoder_libVorbis *this);
void Decoder_libVorbis_Rewind(Decoder_libVorbis *this);
unsigned long Decoder_libVorbis_GetSamples(Decoder_libVorbis *this, void *buffer_void, unsigned long frames_to_do);
