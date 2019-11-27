#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct ResampledDecoderData ResampledDecoderData;
typedef struct ResampledDecoder ResampledDecoder;

ResampledDecoderData* ResampledDecoder_LoadData(const unsigned char *file_buffer, size_t file_size);
void ResampledDecoder_UnloadData(ResampledDecoderData *data);
ResampledDecoder* ResampledDecoder_Create(ResampledDecoderData *data, bool loop, unsigned long sample_rate, unsigned int channel_count);
void ResampledDecoder_Destroy(ResampledDecoder *decoder);
void ResampledDecoder_Rewind(ResampledDecoder *decoder);
unsigned long ResampledDecoder_GetSamples(ResampledDecoder *decoder, void *buffer, unsigned long frames_to_do);
