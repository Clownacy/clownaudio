USE_LIBVORBIS = false
USE_STB_VORBIS = true
USE_LIBFLAC = false
USE_DR_FLAC = true
USE_DR_WAV = true
USE_LIBOPUS = false
USE_LIBSNDFILE = false
USE_LIBOPENMPT = false
USE_LIBXMPLITE = true
USE_SNES_SPC = true
USE_PXTONE = true
# Can be 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', or 'PortAudio'
BACKEND = miniaudio

ifneq ($(RELEASE),)
  CFLAGS = -O2 -flto
else
  CFLAGS = -Og -ggdb
endif
ALL_CFLAGS = -std=c99 -MMD -MP -MF $@.d $(CFLAGS)

ifneq ($(RELEASE),)
  CXXFLAGS = -O2 -flto
else
  CXXFLAGS = -Og -ggdb
endif
ALL_CXXFLAGS = -MMD -MP -MF $@.d $(CXXFLAGS)

ifneq ($(RELEASE),)
  LDFLAGS = -s
else
  LDFLAGS =
endif
ALL_LDFLAGS = $(LDFLAGS)

LIBS =
ALL_LIBS = $(LIBS)

SDL1_CFLAGS = $(shell pkg-config sdl --cflags)
SDL1_LIBS = $(shell pkg-config sdl --libs --static)

SDL2_CFLAGS = $(shell pkg-config sdl2 --cflags)
SDL2_LIBS = $(shell pkg-config sdl2 --libs --static)

SOURCES = \
  test \
  clownaudio/clownaudio \
  clownaudio/miniaudio \
  clownaudio/mixer \
  clownaudio/decoding/decoder_selector \
  clownaudio/decoding/predecoder \
  clownaudio/decoding/resampled_decoder \
  clownaudio/decoding/split_decoder \
  clownaudio/decoding/decoders/memory_stream

ifeq ($(USE_LIBVORBIS), true)
  SOURCES += clownaudio/decoding/decoders/libvorbis
  ALL_CFLAGS += -DUSE_LIBVORBIS $(shell pkg-config vorbisfile --cflags)
  ALL_LIBS += $(shell pkg-config vorbisfile --libs --static)
endif

ifeq ($(USE_STB_VORBIS), true)
  SOURCES += clownaudio/decoding/decoders/stb_vorbis
  ALL_CFLAGS += -DUSE_STB_VORBIS
  ALL_LIBS += -lm
endif

ifeq ($(USE_LIBFLAC), true)
  SOURCES += clownaudio/decoding/decoders/libflac
  ALL_CFLAGS += -DUSE_LIBFLAC $(shell pkg-config flac --cflags)
  ALL_LIBS += $(shell pkg-config flac --libs --static)
endif

ifeq ($(USE_DR_FLAC), true)
  SOURCES += clownaudio/decoding/decoders/dr_flac
  ALL_CFLAGS += -DUSE_DR_FLAC
endif

ifeq ($(USE_DR_WAV), true)
  SOURCES += clownaudio/decoding/decoders/dr_wav
  ALL_CFLAGS += -DUSE_DR_WAV
endif

ifeq ($(USE_LIBOPUS), true)
  SOURCES += clownaudio/decoding/decoders/libopus
  ALL_CFLAGS += -DUSE_LIBOPUS $(shell pkg-config opusfile --cflags)
  ALL_LIBS += $(shell pkg-config opusfile --libs --static)
endif

ifeq ($(USE_LIBSNDFILE), true)
  SOURCES += clownaudio/decoding/decoders/libsndfile
  ALL_CFLAGS += -DUSE_LIBSNDFILE $(shell pkg-config sndfile --cflags)
  ALL_LIBS += $(shell pkg-config sndfile --libs --static)
endif

ifeq ($(USE_LIBOPENMPT), true)
  SOURCES += clownaudio/decoding/decoders/libopenmpt
  ALL_CFLAGS += -DUSE_LIBOPENMPT $(shell pkg-config libopenmpt --cflags)
  ALL_LIBS += $(shell pkg-config libopenmpt --libs --static)
endif

ifeq ($(USE_LIBXMPLITE), true)
  SOURCES += clownaudio/decoding/decoders/libxmp-lite
  ALL_CFLAGS += -DUSE_LIBXMPLITE

  ifeq ($(shell pkg-config libxmp-lite --exists && echo 1), 1)
    ALL_CFLAGS += $(shell pkg-config libxmp-lite --cflags)
    ALL_LIBS += $(shell pkg-config libxmp-lite --libs --static)
  else
    ALL_CFLAGS += -Iclownaudio/decoding/decoders/libs/libxmp-lite/include/libxmp-lite
  endif
endif

ifeq ($(USE_SNES_SPC), true)
  SOURCES += clownaudio/decoding/decoders/snes_spc
  ALL_CFLAGS += -DUSE_SNES_SPC
  ALL_LIBS += -lstdc++
endif

ifeq ($(USE_PXTONE), true)
  SOURCES += clownaudio/decoding/decoders/pxtone clownaudio/decoding/decoders/pxtone_noise
  ALL_CFLAGS += -DUSE_PXTONE
  ALL_LIBS += -lstdc++
  # Apparently PxTone supports Vorbis-encoded samples
  ifeq ($(USE_LIBVORBIS), true)
    ALL_CXXFLAGS += -DpxINCLUDE_OGGVORBIS
  endif
endif

ifeq ($(BACKEND), miniaudio)
  SOURCES += clownaudio/playback/miniaudio
  ALL_CFLAGS += -DMINIAUDIO_ENABLE_DEVICE_IO
  ALL_LIBS += -ldl -lpthread -lm
else ifeq ($(BACKEND), SDL1)
  SOURCES += clownaudio/playback/sdl1
  ALL_CFLAGS += $(SDL1_CFLAGS)
  ALL_LIBS += $(SDL1_LIBS)
else ifeq ($(BACKEND), SDL2)
  SOURCES += clownaudio/playback/sdl2
  ALL_CFLAGS += $(SDL2_CFLAGS)
  ALL_LIBS += $(SDL2_LIBS)
else ifeq ($(BACKEND), Cubeb)
  SOURCES += clownaudio/playback/cubeb
  ALL_LIBS += -lcubeb
else ifeq ($(BACKEND), PortAudio)
  SOURCES += clownaudio/playback/portaudio
  ALL_CFLAGS += $(shell pkg-config portaudio-2.0 --cflags)
  ALL_LIBS += $(shell pkg-config portaudio-2.0 --libs --static)
endif

LIBXMPLITE_SOURCES = \
  src/virtual \
  src/format \
  src/period \
  src/player \
  src/read_event \
  src/dataio \
  src/lfo \
  src/scan \
  src/control \
  src/filter \
  src/effects \
  src/mixer \
  src/mix_all \
  src/load_helpers \
  src/load \
  src/hio \
  src/smix \
  src/memio \
  src/loaders/common \
  src/loaders/itsex \
  src/loaders/sample \
  src/loaders/xm_load \
  src/loaders/mod_load \
  src/loaders/s3m_load \
  src/loaders/it_load

SPC_SOURCES = \
  dsp \
  SNES_SPC \
  SNES_SPC_misc \
  SNES_SPC_state \
  spc \
  SPC_DSP \
  SPC_Filter

PXTONE_SOURCES = \
  pxtnDelay \
  pxtnDescriptor \
  pxtnError \
  pxtnEvelist \
  pxtnMaster \
  pxtnMem \
  pxtnOverDrive \
  pxtnPulse_Frequency \
  pxtnPulse_Noise \
  pxtnPulse_NoiseBuilder \
  pxtnPulse_Oggv \
  pxtnPulse_Oscillator \
  pxtnPulse_PCM \
  pxtnService \
  pxtnService_moo \
  pxtnText \
  pxtnUnit \
  pxtnWoice \
  pxtnWoice_io \
  pxtnWoicePTV \
  pxtoneNoise

OBJECTS += $(addprefix obj/main/, $(addsuffix .o, $(SOURCES)))
ifeq ($(USE_LIBXMPLITE), true)
  ifneq ($(shell pkg-config libxmp-lite --exists && echo 1), 1)
    OBJECTS += $(addprefix obj/libxmp-lite/, $(addsuffix .o, $(LIBXMPLITE_SOURCES)))
  endif
endif
ifeq ($(USE_SNES_SPC), true)
  OBJECTS += $(addprefix obj/spc/, $(addsuffix .o, $(SPC_SOURCES)))
endif
ifeq ($(USE_PXTONE), true)
  OBJECTS += $(addprefix obj/pxtone/, $(addsuffix .o, $(PXTONE_SOURCES)))
endif

DEPENDENCIES = $(addsuffix .d, $(OBJECTS))

all: test

obj/main/%.o: %.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra -pedantic $< -o $@ -c

obj/main/%.o: %.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=c++11 -Wall -Wextra -pedantic $< -o $@ -c

obj/libxmp-lite/%.o: clownaudio/decoding/decoders/libs/libxmp-lite/%.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -std=gnu11 -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-sign-compare -Wno-maybe-uninitialized -Iclownaudio/decoding/decoders/libs/libxmp-lite/src -DLIBXMP_CORE_PLAYER=1 -Dinline=__inline -D_USE_MATH_DEFINES=1 -DBUILDING_STATIC=1 $< -o $@ -c

obj/spc/%.o: clownaudio/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=c++98 $< -o $@ -c

obj/pxtone/%.o: clownaudio/decoding/decoders/libs/pxtone/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=c++11 -Wall -Wextra -pedantic $< -o $@ -c

test: $(OBJECTS)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra -pedantic $^ -o $@ $(ALL_LDFLAGS) $(ALL_LIBS)

include $(wildcard $(DEPENDENCIES))
