// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "snes_spc.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "libs/snes_spc-0.9.0/snes_spc/spc.h"

#include "common.h"
#include "memory_file.h"

typedef struct DecoderData_SNES_SPC
{
	unsigned char *file_buffer;
	size_t file_size;
} DecoderData_SNES_SPC;

typedef struct Decoder_SNES_SPC
{
	DecoderData_SNES_SPC *data;
	SNES_SPC *snes_spc;
//	SPC_Filter *filter;
} Decoder_SNES_SPC;

DecoderData_SNES_SPC* Decoder_SNES_SPC_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_SNES_SPC *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_SNES_SPC));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_SNES_SPC_UnloadData(DecoderData_SNES_SPC *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_SNES_SPC* Decoder_SNES_SPC_Create(DecoderData_SNES_SPC *data, bool loop, DecoderInfo *info)
{
	(void)loop;	// Unusable, sadly

	Decoder_SNES_SPC *this = NULL;

	SNES_SPC *snes_spc = spc_new();

	//SPC_Filter *filter = spc_filter_new();

	if (!spc_load_spc(snes_spc, data->file_buffer, data->file_size))
	{
		spc_clear_echo(snes_spc);

	//	spc_filter_clear(filter);

		this = malloc(sizeof(Decoder_SNES_SPC));
		this->data = data;
		this->snes_spc = snes_spc;
	//	this->filter = filter;

		info->sample_rate = spc_sample_rate;
		info->channel_count = 2;
		info->format = DECODER_FORMAT_S16;
	}

	return this;
}

void Decoder_SNES_SPC_Destroy(Decoder_SNES_SPC *this)
{
	if (this)
	{
		spc_delete(this->snes_spc);
		free(this);
	}
}

void Decoder_SNES_SPC_Rewind(Decoder_SNES_SPC *this)
{
	spc_delete(this->snes_spc);
	spc_load_spc(this->snes_spc, this->data->file_buffer, this->data->file_size);
}

unsigned long Decoder_SNES_SPC_GetSamples(Decoder_SNES_SPC *this, void *buffer, unsigned long frames_to_do)
{
	spc_play(this->snes_spc, frames_to_do * 2, buffer);

//	spc_filter_run(this->filter, buffer, bytes_to_do / sizeof(short));

	return frames_to_do;
}
