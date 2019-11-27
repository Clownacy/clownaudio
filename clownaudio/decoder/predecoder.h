#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct PredecoderData PredecoderData;
typedef struct Predecoder Predecoder;

PredecoderData* Predecoder_DecodeData(const unsigned char *file_buffer, size_t file_size, unsigned long sample_rate, unsigned int channel_count);
void Predecoder_UnloadData(PredecoderData *data);
Predecoder* Predecoder_Create(PredecoderData *data, bool loop);
void Predecoder_Destroy(Predecoder *predecoder);
void Predecoder_Rewind(Predecoder *predecoder);
unsigned long Predecoder_GetSamples(Predecoder *predecoder, void *buffer_void, unsigned long frames_to_do);
