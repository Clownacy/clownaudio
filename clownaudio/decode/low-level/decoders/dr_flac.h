#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "../../common.h"

typedef struct Decoder_DR_FLAC Decoder_DR_FLAC;

Decoder_DR_FLAC* Decoder_DR_FLAC_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_DR_FLAC_Destroy(Decoder_DR_FLAC *decoder);
void Decoder_DR_FLAC_Rewind(Decoder_DR_FLAC *decoder);
size_t Decoder_DR_FLAC_GetSamples(Decoder_DR_FLAC *decoder, void *buffer, size_t frames_to_do);
