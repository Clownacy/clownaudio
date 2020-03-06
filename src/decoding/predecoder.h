/*
 *  (C) 2019-2020 Clownacy
 *
 *  This software is provided 'as-is', without any express or implied
 *  warranty.  In no event will the authors be held liable for any damages
 *  arising from the use of this software.
 *
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *
 *  1. The origin of this software must not be misrepresented; you must not
 *     claim that you wrote the original software. If you use this software
 *     in a product, an acknowledgment in the product documentation would be
 *     appreciated but is not required.
 *  2. Altered source versions must be plainly marked as such, and must not be
 *     misrepresented as being the original software.
 *  3. This notice may not be removed or altered from any source distribution.
 */

#pragma once

#include <stddef.h>

#include "bool.h"

#include "decoders/common.h"

typedef struct PredecoderData PredecoderData;
typedef struct Predecoder Predecoder;

PredecoderData* Predecoder_DecodeData(DecoderInfo *info, void *decoder, size_t (*decoder_get_samples_function)(void *decoder, void *buffer, size_t frames_to_do));
void Predecoder_UnloadData(PredecoderData *data);
Predecoder* Predecoder_Create(PredecoderData *data, CA_BOOL loop, DecoderInfo *info);
void Predecoder_Destroy(Predecoder *predecoder);
void Predecoder_Rewind(Predecoder *predecoder);
size_t Predecoder_GetSamples(Predecoder *predecoder, void *buffer, size_t frames_to_do);
void Predecoder_SetLoop(Predecoder *predecoder, CA_BOOL loop);
