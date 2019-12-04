#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "../common.h"

typedef struct LowLevelDecoderSelector LowLevelDecoderSelector;

LowLevelDecoderSelector* LowLevelDecoderSelector_Create(const unsigned char *data, size_t data_size, bool loop, DecoderInfo *info);
void LowLevelDecoderSelector_Destroy(LowLevelDecoderSelector *selector);
void LowLevelDecoderSelector_Rewind(LowLevelDecoderSelector *selector);
unsigned long LowLevelDecoderSelector_GetSamples(LowLevelDecoderSelector *selector, void *buffer, unsigned long frames_to_do);
