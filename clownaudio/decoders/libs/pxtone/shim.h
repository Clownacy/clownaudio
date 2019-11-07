#pragma once

#include <stdbool.h>

typedef struct pxtnService pxtnService;

pxtnService* PxTone_Open(const unsigned char *file_buffer, size_t file_size, bool loop, unsigned int sample_rate, unsigned int channel_count);
void PxTone_Close(pxtnService *decoder);
void PxTone_Rewind(pxtnService *decoder, bool loop);
unsigned long PxTone_GetSamples(pxtnService *decoder, void *buffer, unsigned long bytes_to_do);

bool PxTone_NoiseGenerate(const unsigned char *file_buffer, size_t file_size, unsigned int sample_rate, unsigned int channel_count, void** buffer, size_t *buffer_size);
