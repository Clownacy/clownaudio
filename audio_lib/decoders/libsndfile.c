// Alternate music mod for 2004 Cave Story
// Copyright © 2018 Clownacy

#include "libsndfile.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include <sndfile.h>

#include "common.h"
#include "memory_file.h"

struct DecoderData_libSndfile
{
	unsigned char *file_buffer;
	size_t file_size;
};

struct Decoder_libSndfile
{
	DecoderData_libSndfile *data;
	MemoryFile *file;
	SNDFILE *sndfile;
	DecoderFormat format;
	bool loops;
	unsigned int channel_count;
};

static sf_count_t MemoryFile_fread_wrapper(void *output, sf_count_t count, void *user)
{
	return MemoryFile_fread(output, 1, count, user);
}

static sf_count_t MemoryFile_fseek_wrapper(sf_count_t offset, int origin, void *user)
{
	return MemoryFile_fseek(user, offset, origin);
}

static sf_count_t MemoryFile_ftell_wrapper(void *user)
{
	return MemoryFile_ftell(user);
}

static sf_count_t MemoryFile_GetSize(void *user)
{
	const sf_count_t old_offset = MemoryFile_ftell_wrapper(user);

	MemoryFile_fseek_wrapper(0, SEEK_END, user);
	const sf_count_t size = MemoryFile_ftell_wrapper(user);

	MemoryFile_fseek_wrapper(old_offset, SEEK_SET, user);

	return size;
}

static SF_VIRTUAL_IO sfvirtual = {
	MemoryFile_GetSize,
	MemoryFile_fseek_wrapper,
	MemoryFile_fread_wrapper,
	NULL,
	MemoryFile_ftell_wrapper
};

DecoderData_libSndfile* Decoder_libSndfile_LoadData(const char *file_path, LinkedBackend *linked_backend)
{
	(void)linked_backend;

	DecoderData_libSndfile *data = NULL;

	size_t file_size;
	unsigned char *file_buffer = MemoryFile_fopen_to(file_path, &file_size);

	if (file_buffer)
	{
		data = malloc(sizeof(DecoderData_libSndfile));
		data->file_buffer = file_buffer;
		data->file_size = file_size;
	}

	return data;
}

void Decoder_libSndfile_UnloadData(DecoderData_libSndfile *data)
{
	if (data)
	{
		free(data->file_buffer);
		free(data);
	}
}

Decoder_libSndfile* Decoder_libSndfile_Create(DecoderData_libSndfile *data, bool loops, DecoderInfo *info)
{
	Decoder_libSndfile *decoder = NULL;

	MemoryFile *file = MemoryFile_fopen_from(data->file_buffer, data->file_size, false);

	SF_INFO sf_info;
	memset(&sf_info, 0, sizeof(SF_INFO));

	SNDFILE *sndfile = sf_open_virtual(&sfvirtual, SFM_READ, &sf_info, file);

	if (sndfile)
	{
		decoder = malloc(sizeof(Decoder_libSndfile));
		decoder->data = data;
		decoder->sndfile = sndfile;
		decoder->file = file;
		decoder->channel_count = sf_info.channels;
		decoder->loops = loops;

		info->sample_rate = sf_info.samplerate;
		info->channel_count = sf_info.channels;
		info->decoded_size = sf_info.frames * sf_info.channels * sizeof(float);
		info->format = DECODER_FORMAT_F32;
	}
	else
	{
		MemoryFile_fclose(file);
	}

	return decoder;
}

void Decoder_libSndfile_Destroy(Decoder_libSndfile *decoder)
{
	if (decoder)
	{
		sf_close(decoder->sndfile);
		MemoryFile_fclose(decoder->file);
		free(decoder);
	}
}

void Decoder_libSndfile_Rewind(Decoder_libSndfile *decoder)
{
	sf_seek(decoder->sndfile, 0, SEEK_SET);
}

unsigned long Decoder_libSndfile_GetSamples(Decoder_libSndfile *decoder, void *output_buffer_void, unsigned long frames_to_do)
{
	float *output_buffer = output_buffer_void;

	unsigned long frames_done_total = 0;

	for (unsigned long frames_done; frames_done_total < frames_to_do; frames_done_total += frames_done)
	{
		frames_done = sf_readf_float(decoder->sndfile, output_buffer + (frames_done_total * decoder->channel_count), frames_to_do - frames_done_total);

		if (frames_done < frames_to_do - frames_done_total)
		{
			if (decoder->loops)
				Decoder_libSndfile_Rewind(decoder);
			else
				break;
		}
	}

	return frames_done_total;
}
