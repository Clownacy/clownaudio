// Copyright (c) 2019-2020 Clownacy
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef PREDECODER_H
#define PREDECODER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

#include "decoders/common.h"

typedef struct PredecoderData PredecoderData;

PredecoderData* Predecoder_DecodeData(const DecoderSpec *in_spec, const DecoderSpec *out_spec, DecoderStage *stage);
void Predecoder_UnloadData(PredecoderData *data);
void* Predecoder_Create(PredecoderData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec);
void Predecoder_Destroy(void *predecoder);
void Predecoder_Rewind(void *predecoder);
size_t Predecoder_GetSamples(void *predecoder, short *buffer, size_t frames_to_do);
void Predecoder_SetLoop(void *predecoder, bool loop);

#endif // PREDECODER_H
