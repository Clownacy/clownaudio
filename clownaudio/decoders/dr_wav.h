#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_DR_WAV Decoder_DR_WAV;

Decoder_DR_WAV* Decoder_DR_WAV_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_DR_WAV_Destroy(Decoder_DR_WAV *decoder);
void Decoder_DR_WAV_Rewind(Decoder_DR_WAV *decoder);
unsigned long Decoder_DR_WAV_GetSamples(Decoder_DR_WAV *decoder, void *buffer_void, unsigned long frames_to_do);
