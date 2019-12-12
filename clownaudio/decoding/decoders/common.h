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
