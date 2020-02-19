#pragma once

#include <stdbool.h>

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
	bool complex;
} DecoderInfo;

typedef struct Decoder Decoder;
