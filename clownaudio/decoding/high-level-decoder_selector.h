#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "decoders/common.h"

typedef struct HighLevelDecoderSelector HighLevelDecoderSelector;

HighLevelDecoderSelector* HighLevelDecoderSelector_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void HighLevelDecoderSelector_Destroy(HighLevelDecoderSelector *selector);
void HighLevelDecoderSelector_Rewind(HighLevelDecoderSelector *selector);
size_t HighLevelDecoderSelector_GetSamples(HighLevelDecoderSelector *selector, void *buffer, size_t frames_to_do);
