#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum DecoderFormat
{
	DECODER_FORMAT_S16,
	DECODER_FORMAT_S32,
	DECODER_FORMAT_F32
} DecoderFormat;

typedef struct DecoderInfo
{
	unsigned long sample_rate;
	unsigned int channel_count;
	DecoderFormat format;
} DecoderInfo;

typedef struct HighLevelDecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} HighLevelDecoderFunctions;

typedef struct LowLevelDecoderFunctions
{
	void* (*Create)(const unsigned char *data, size_t data_size, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, void *buffer, size_t frames_to_do);
} LowLevelDecoderFunctions;
