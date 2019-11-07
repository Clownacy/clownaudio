#include "shim.h"

#include <cstddef>

#include "pxtnService.h"
#include "pxtnError.h"
#include "pxtoneNoise.h"

static bool _load_ptcop(pxtnService* pxtn, const unsigned char *file_buffer, size_t file_size)
{
	bool success = false;

	pxtnDescriptor desc;
	if (desc.set_memory_r((void*)file_buffer, file_size) && pxtn->read(&desc) == pxtnOK && pxtn->tones_ready() == pxtnOK)
		success = true;
	else
		pxtn->evels->Release();

	return success;
}

pxtnService* PxTone_Open(const unsigned char *file_buffer, size_t file_size, bool loop, unsigned int sample_rate, unsigned int channel_count)
{
	pxtnService *pxtn = new pxtnService();
	if (pxtn->init() == pxtnOK)
	{
		if (pxtn->set_destination_quality(channel_count, sample_rate))
		{
			if( _load_ptcop( pxtn, file_buffer, file_size ) )
			{
				pxtnVOMITPREPARATION prep = pxtnVOMITPREPARATION();
				if (loop)
					prep.flags |= pxtnVOMITPREPFLAG_loop;
				prep.start_pos_float = 0;
				prep.master_volume = 0.80f;

				if( pxtn->moo_preparation( &prep ) )
				{
					return pxtn;
				}
			}
		}
	}

	delete pxtn;

	return NULL;
}

void PxTone_Close(pxtnService *pxtn)
{
	delete pxtn;
}

void PxTone_Rewind(pxtnService *pxtn, bool loop)
{
	pxtnVOMITPREPARATION prep = {};
	if (loop)
		prep.flags |= pxtnVOMITPREPFLAG_loop;
	prep.start_pos_float = 0;
	prep.master_volume = 0.80f;

	pxtn->moo_preparation( &prep );
}

unsigned long PxTone_GetSamples(pxtnService *pxtn, void *buffer, unsigned long bytes_to_do)
{
	return pxtn->Moo(buffer, bytes_to_do);
}

bool PxTone_NoiseGenerate(const unsigned char *file_buffer, size_t file_size, unsigned int sample_rate, unsigned int channel_count, void** buffer, size_t *buffer_size)
{
	bool success = false;

	pxtoneNoise *pxtn = new pxtoneNoise();

	if (pxtn->init())
	{
		if (pxtn->quality_set(channel_count, sample_rate, 16))
		{
			pxtnDescriptor desc;

			if (desc.set_memory_r((void*)file_buffer, file_size))
			{
				int32_t size_int;

				if (pxtn->generate(&desc, buffer, &size_int))
				{
					*buffer_size = size_int;
					success = true;
				}
			}
		}
	}

	delete pxtn;

	return success;
}
