// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "dr_wav.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO

#include "libs/dr_wav.h"

#include "common.h"
#include "memory_file.h"

typedef struct DecoderData_DR_WAV
{
	unsigned char *file_buffer;
	size_t file_size;
} DecoderData_DR_WAV;

typedef struct Decoder_DR_WAV
{
	DecoderData_DR_WAV *data;
	drwav *instance;
	bool loops;
} Decoder_DR_WAV;

DecoderData_DR_WAV* Decoder_DR_WAV_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_DR_WAV *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_DR_WAV));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_DR_WAV_UnloadData(DecoderData_DR_WAV *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_DR_WAV* Decoder_DR_WAV_Create(DecoderData_DR_WAV *data, bool loops, DecoderInfo *info)
{
	Decoder_DR_WAV *this = NULL;

	if (data && data->file_buffer)
	{
		drwav *instance = drwav_open_memory(data->file_buffer, data->file_size);

		if (instance)
		{
			this = malloc(sizeof(Decoder_DR_WAV));

			this->instance = instance;
			this->data = data;
			this->loops = loops;

			info->sample_rate = instance->sampleRate;
			info->channel_count = instance->channels;
			info->decoded_size = (unsigned long)instance->totalPCMFrameCount * instance->channels * sizeof(float);
			info->format = DECODER_FORMAT_F32;
		}
	}

	return this;
}

void Decoder_DR_WAV_Destroy(Decoder_DR_WAV *this)
{
	if (this)
	{
		drwav_uninit(this->instance);
		free(this->instance);
		free(this);
	}
}

void Decoder_DR_WAV_Rewind(Decoder_DR_WAV *this)
{
	drwav_seek_to_pcm_frame(this->instance, 0);
}

unsigned long Decoder_DR_WAV_GetSamples(Decoder_DR_WAV *this, void *buffer_void, unsigned long frames_to_do)
{
	float *buffer = buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total != frames_to_do; frames_done_total += frames_done)
	{
		frames_done = (unsigned long)drwav_read_pcm_frames_f32(this->instance, frames_to_do - frames_done_total, buffer + (frames_done_total * this->instance->channels));

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (this->loops)
				Decoder_DR_WAV_Rewind(this);
			else
				break;
		}
	}

	return frames_done_total;
}
