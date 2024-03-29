// Copyright (c) 2019-2021 Clownacy
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

#ifndef RESAMPLED_DECODER_H
#define RESAMPLED_DECODER_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

#include "decoders/common.h"

void* ResampledDecoder_Create(DecoderStage *next_stage, bool dynamic_sample_rate, const DecoderSpec *wanted_spec, const DecoderSpec *child_spec);
void ResampledDecoder_Destroy(void *resampled_decoder);
void ResampledDecoder_Rewind(void *resampled_decoder);
size_t ResampledDecoder_GetSamples(void *resampled_decoder, short *buffer, size_t frames_to_do);
void ResampledDecoder_SetLoop(void *resampled_decoder, bool loop);
void ResampledDecoder_SetSpeed(void *resampled_decoder, unsigned long speed);
void ResampledDecoder_SetLowPassFilter(void *resampled_decoder, unsigned long low_pass_filter_sample_rate);

#endif
