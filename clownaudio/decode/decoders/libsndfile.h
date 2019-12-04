#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "../common.h"

typedef struct Decoder_libSndfile Decoder_libSndfile;

Decoder_libSndfile* Decoder_libSndfile_Create(const unsigned char *data, size_t data_size, DecoderInfo *info);
void Decoder_libSndfile_Destroy(Decoder_libSndfile *decoder);
void Decoder_libSndfile_Rewind(Decoder_libSndfile *decoder);
size_t Decoder_libSndfile_GetSamples(Decoder_libSndfile *decoder, void *buffer, size_t frames_to_do);
