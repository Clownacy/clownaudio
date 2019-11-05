#include <cstddef>

#include "pxtnService.h"
#include "pxtnError.h"

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

extern "C" pxtnService* PxTone_Open(const unsigned char *file_buffer, size_t file_size, bool loop, unsigned int sample_rate, unsigned int channel_count)
{
	pxtnService *pxtn = new pxtnService();
	if (pxtn->init() == pxtnOK)
	{
		if (pxtn->set_destination_quality(channel_count, sample_rate))
		{
			if( _load_ptcop( pxtn, file_buffer, file_size ) )
			{
				pxtnVOMITPREPARATION prep = {};
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

extern "C" void PxTone_Close(pxtnService *pxtn)
{
	delete pxtn;
}

extern "C" void PxTone_Rewind(pxtnService *pxtn, bool loop)
{
	pxtnVOMITPREPARATION prep = {};
	if (loop)
		prep.flags |= pxtnVOMITPREPFLAG_loop;
	prep.start_pos_float = 0;
	prep.master_volume = 0.80f;

	pxtn->moo_preparation( &prep );
}

extern "C" unsigned long PxTone_GetSamples(pxtnService *pxtn, void *buffer, unsigned long bytes_to_do)
{
	pxtn->Moo(buffer, bytes_to_do);

	return bytes_to_do;
}
