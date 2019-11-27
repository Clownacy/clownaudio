#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct SplitDecoderData SplitDecoderData;
typedef struct SplitDecoder SplitDecoder;

SplitDecoderData* SplitDecoder_DecodeData(const unsigned char *file_buffer1, size_t file_size1, const unsigned char *file_buffer2, size_t file_size2, unsigned long sample_rate, unsigned int channel_count);
void SplitDecoder_UnloadData(SplitDecoderData *data);
SplitDecoder* SplitDecoder_Create(SplitDecoderData *data, bool loop);
void SplitDecoder_Destroy(SplitDecoder *split_decoder);
void SplitDecoder_Rewind(SplitDecoder *split_decoder);
unsigned long SplitDecoder_GetSamples(SplitDecoder *split_decoder, void *buffer, unsigned long frames_to_do);
