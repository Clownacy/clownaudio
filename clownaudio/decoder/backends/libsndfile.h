#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct Decoder_libSndfile Decoder_libSndfile;

Decoder_libSndfile* Decoder_libSndfile_Create(DecoderData *data, bool loop, DecoderInfo *info);
void Decoder_libSndfile_Destroy(Decoder_libSndfile *decoder);
void Decoder_libSndfile_Rewind(Decoder_libSndfile *decoder);
unsigned long Decoder_libSndfile_GetSamples(Decoder_libSndfile *decoder, void *buffer, unsigned long frames_to_do);
