#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef enum DecoderFormat
{
	DECODER_FORMAT_S16,
	DECODER_FORMAT_S32,
	DECODER_FORMAT_F32
} DecoderFormat;

typedef struct DecoderData
{
	const unsigned char *file_buffer;
	size_t file_size;
} DecoderData;

typedef struct DecoderInfo
{
	unsigned long sample_rate;
	unsigned int channel_count;
	size_t decoded_size;
	DecoderFormat format;
} DecoderInfo;

typedef struct DecoderBackend
{
	void* (*Create)(DecoderData *data, bool loops, unsigned long sample_rate, unsigned int channel_count, DecoderInfo *info);
	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	unsigned long (*GetSamples)(void *decoder, void *buffer_void, unsigned long frames_to_do);
} DecoderBackend;

static inline unsigned int GetSizeOfFrame(DecoderInfo *info)
{
	const unsigned int sizes[3] = {sizeof(short), sizeof(long), sizeof(float)};

	return sizes[info->format] * info->channel_count;
}
