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

#ifndef COMMON_H
#define COMMON_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

typedef struct DecoderSpec
{
	unsigned long sample_rate;
	unsigned int channel_count;
	bool is_complex;
} DecoderSpec;

typedef struct DecoderStage
{
	void *decoder;

	void (*Destroy)(void *decoder);
	void (*Rewind)(void *decoder);
	size_t (*GetSamples)(void *decoder, short *buffer, size_t frames_to_do);
	void (*SetLoop)(void *decoder, bool loop);
} DecoderStage;

#endif // COMMON_H
