/*
Copyright (c) 2023 Ned Loynd

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef DECODER_OSWRAPPER_AUDIO_H
#define DECODER_OSWRAPPER_AUDIO_H

#ifndef __cplusplus
#include <stdbool.h>
#endif
#include <stddef.h>

#include "common.h"

void* Decoder_OSWRAPPER_AUDIO_Create(const unsigned char *data, size_t data_size, bool loop, const DecoderSpec *wanted_spec, DecoderSpec *spec);
void Decoder_OSWRAPPER_AUDIO_Destroy(void *decoder);
void Decoder_OSWRAPPER_AUDIO_Rewind(void *decoder);
size_t Decoder_OSWRAPPER_AUDIO_GetSamples(void *decoder, short *buffer, size_t frames_to_do);

#endif /* DECODER_OSWRAPPER_AUDIO_H */
