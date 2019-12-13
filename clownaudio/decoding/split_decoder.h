#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct SplitDecoderData SplitDecoderData;
typedef struct SplitDecoder SplitDecoder;

SplitDecoderData* SplitDecoder_LoadData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, bool predecode);
void SplitDecoder_UnloadData(SplitDecoderData *data);
SplitDecoder* SplitDecoder_Create(SplitDecoderData *data, bool loop, unsigned long sample_rate);
void SplitDecoder_Destroy(SplitDecoder *split_decoder);
void SplitDecoder_Rewind(SplitDecoder *split_decoder);
size_t SplitDecoder_GetSamples(SplitDecoder *split_decoder, void *buffer, size_t frames_to_do);
void SplitDecoder_SetLoop(SplitDecoder *split_decoder, bool loop);
void SplitDecoder_SetSampleRate(SplitDecoder *split_decoder, unsigned long sample_rate1, unsigned long sample_rate2);
