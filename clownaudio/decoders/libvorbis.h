#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_libVorbis Decoder_libVorbis;

Decoder_libVorbis* Decoder_libVorbis_Create(DecoderData *data, bool loops, unsigned int sample_rate, unsigned int channel_count, DecoderInfo *info);
void Decoder_libVorbis_Destroy(Decoder_libVorbis *decoder);
void Decoder_libVorbis_Rewind(Decoder_libVorbis *decoder);
unsigned long Decoder_libVorbis_GetSamples(Decoder_libVorbis *decoder, void *buffer_void, unsigned long frames_to_do);
