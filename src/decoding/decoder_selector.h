// Copyright (c) 2019 Clownacy
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

#ifndef DECODER_SELECTOR_H
#define DECODER_SELECTOR_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

#include "decoders/common.h"

typedef struct DecoderSelectorData DecoderSelectorData;

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *data, size_t data_size, bool predecode, bool must_predecode, const DecoderSpec *wanted_spec);
void DecoderSelector_UnloadData(DecoderSelectorData *data);
void* DecoderSelector_Create(DecoderSelectorData *data, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec);
void DecoderSelector_Destroy(void *selector);
void DecoderSelector_Rewind(void *selector);
size_t DecoderSelector_GetSamples(void *selector, short *buffer, size_t frames_to_do);
void DecoderSelector_SetLoop(void *selector, bool loop);

#endif // DECODER_SELECTOR_H
