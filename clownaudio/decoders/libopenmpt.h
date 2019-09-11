#pragma once

#include <stdbool.h>

#include "common.h"

typedef struct DecoderData_libOpenMPT DecoderData_libOpenMPT;
typedef struct Decoder_libOpenMPT Decoder_libOpenMPT;

DecoderData_libOpenMPT* Decoder_libOpenMPT_LoadData(const char *file_path, LinkedBackend *linked_backend);
void Decoder_libOpenMPT_UnloadData(DecoderData_libOpenMPT *data);
Decoder_libOpenMPT* Decoder_libOpenMPT_Create(DecoderData_libOpenMPT *data, bool loops, DecoderInfo *info);
void Decoder_libOpenMPT_Destroy(Decoder_libOpenMPT *this);
void Decoder_libOpenMPT_Rewind(Decoder_libOpenMPT *this);
unsigned long Decoder_libOpenMPT_GetSamples(Decoder_libOpenMPT *this, void *buffer, unsigned long bytes_to_do);
