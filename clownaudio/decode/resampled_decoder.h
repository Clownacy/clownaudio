#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct DecoderSelectorData ResampledDecoderData;	// This is deliberate
typedef struct ResampledDecoder ResampledDecoder;

ResampledDecoderData* ResampledDecoder_LoadData(const unsigned char *file_buffer, size_t file_size, bool predecode);
void ResampledDecoder_UnloadData(ResampledDecoderData *data);
ResampledDecoder* ResampledDecoder_Create(ResampledDecoderData *data, bool loop, unsigned long sample_rate);
void ResampledDecoder_Destroy(ResampledDecoder *resampled_decoder);
void ResampledDecoder_Rewind(ResampledDecoder *resampled_decoder);
size_t ResampledDecoder_GetSamples(ResampledDecoder *resampled_decoder, void *buffer, size_t frames_to_do);
void ResampledDecoder_SetSampleRate(ResampledDecoder *resampled_decoder, unsigned long sample_rate);
