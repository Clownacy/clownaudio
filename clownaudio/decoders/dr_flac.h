#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "common.h"

typedef struct Decoder_DR_FLAC Decoder_DR_FLAC;

Decoder_DR_FLAC* Decoder_DR_FLAC_Create(DecoderData *data, bool loop, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_DR_FLAC_Destroy(Decoder_DR_FLAC *decoder);
void Decoder_DR_FLAC_Rewind(Decoder_DR_FLAC *decoder);
unsigned long Decoder_DR_FLAC_GetSamples(Decoder_DR_FLAC *decoder, void *buffer_void, unsigned long frames_to_do);
