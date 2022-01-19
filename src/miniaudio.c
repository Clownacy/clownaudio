#define MINIAUDIO_IMPLEMENTATION

#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_VORBIS
#define MA_NO_GENERATION
#define MA_NO_ENGINE
#define MA_NO_NODE_GRAPH

#ifndef CLOWNAUDIO_MINIAUDIO_ENABLE_DEVICE_IO
 #define MA_NO_DEVICE_IO
 #define MA_NO_THREADING
#endif

#include "miniaudio.h"
