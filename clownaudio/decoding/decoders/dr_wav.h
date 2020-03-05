#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_DR_WAV Decoder_DR_WAV;

Decoder_DR_WAV* Decoder_DR_WAV_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void Decoder_DR_WAV_Destroy(Decoder_DR_WAV *decoder);
void Decoder_DR_WAV_Rewind(Decoder_DR_WAV *decoder);
size_t Decoder_DR_WAV_GetSamples(Decoder_DR_WAV *decoder, void *buffer, size_t frames_to_do);
