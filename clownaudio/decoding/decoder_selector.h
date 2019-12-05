#pragma once

#include <stdbool.h>
#include <stddef.h>

#include "decoders/common.h"

typedef struct DecoderSelectorData DecoderSelectorData;
typedef struct DecoderSelector DecoderSelector;

DecoderSelectorData* DecoderSelector_LoadData(const unsigned char *data, size_t data_size, bool predecode);
void DecoderSelector_UnloadData(DecoderSelectorData *data);
DecoderSelector* DecoderSelector_Create(DecoderSelectorData *data, bool loop, DecoderInfo *info);
void DecoderSelector_Destroy(DecoderSelector *selector);
void DecoderSelector_Rewind(DecoderSelector *selector);
size_t DecoderSelector_GetSamples(DecoderSelector *selector, void *buffer, size_t frames_to_do);
