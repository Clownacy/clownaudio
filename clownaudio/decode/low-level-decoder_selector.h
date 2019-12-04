#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "decoders/common.h"

typedef struct LowLevelDecoderSelector LowLevelDecoderSelector;

LowLevelDecoderSelector* LowLevelDecoderSelector_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void LowLevelDecoderSelector_Destroy(LowLevelDecoderSelector *selector);
void LowLevelDecoderSelector_Rewind(LowLevelDecoderSelector *selector);
size_t LowLevelDecoderSelector_GetSamples(LowLevelDecoderSelector *selector, void *buffer, size_t frames_to_do);
