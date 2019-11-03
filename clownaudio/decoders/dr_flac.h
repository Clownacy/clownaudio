#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct DecoderData_DR_FLAC DecoderData_DR_FLAC;
typedef struct Decoder_DR_FLAC Decoder_DR_FLAC;

DecoderData_DR_FLAC* Decoder_DR_FLAC_LoadData(const unsigned char *file_buffer, size_t file_size);
void Decoder_DR_FLAC_UnloadData(DecoderData_DR_FLAC *data);
Decoder_DR_FLAC* Decoder_DR_FLAC_Create(DecoderData_DR_FLAC *data, bool loops, DecoderInfo *info);
void Decoder_DR_FLAC_Destroy(Decoder_DR_FLAC *this);
void Decoder_DR_FLAC_Rewind(Decoder_DR_FLAC *this);
unsigned long Decoder_DR_FLAC_GetSamples(Decoder_DR_FLAC *this, void *buffer_void, unsigned long frames_to_do);
