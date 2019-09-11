#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_libSndfile DecoderData_libSndfile;
typedef struct Decoder_libSndfile Decoder_libSndfile;

DecoderData_libSndfile* Decoder_libSndfile_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_libSndfile_UnloadData(DecoderData_libSndfile *data);
Decoder_libSndfile* Decoder_libSndfile_Create(DecoderData_libSndfile *data, bool loops, DecoderInfo *info);
void Decoder_libSndfile_Destroy(Decoder_libSndfile *this);
void Decoder_libSndfile_Rewind(Decoder_libSndfile *this);
unsigned long Decoder_libSndfile_GetSamples(Decoder_libSndfile *this, void *buffer, unsigned long bytes_to_do);
