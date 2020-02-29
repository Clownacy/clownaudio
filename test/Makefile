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

CLOWNAUDIO_DIR = ..

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
  clownaudio \
  miniaudio \
  mixer \
  decoding/decoder_selector \
  decoding/predecoder \
  decoding/resampled_decoder \
  decoding/split_decoder \
  decoding/decoders/memory_stream

ifeq ($(USE_LIBVORBIS), true)
  SOURCES += decoding/decoders/libvorbis
  ALL_CFLAGS += -DUSE_LIBVORBIS $(shell pkg-config vorbisfile --cflags)
  ALL_LIBS += $(shell pkg-config vorbisfile --libs --static)
endif

ifeq ($(USE_STB_VORBIS), true)
  SOURCES += decoding/decoders/stb_vorbis
  ALL_CFLAGS += -DUSE_STB_VORBIS
  ALL_LIBS += -lm
endif

ifeq ($(USE_LIBFLAC), true)
  SOURCES += decoding/decoders/libflac
  ALL_CFLAGS += -DUSE_LIBFLAC $(shell pkg-config flac --cflags)
  ALL_LIBS += $(shell pkg-config flac --libs --static)
endif

ifeq ($(USE_DR_FLAC), true)
  SOURCES += decoding/decoders/dr_flac
  ALL_CFLAGS += -DUSE_DR_FLAC
endif

ifeq ($(USE_DR_WAV), true)
  SOURCES += decoding/decoders/dr_wav
  ALL_CFLAGS += -DUSE_DR_WAV
endif

ifeq ($(USE_LIBOPUS), true)
  SOURCES += decoding/decoders/libopus
  ALL_CFLAGS += -DUSE_LIBOPUS $(shell pkg-config opusfile --cflags)
  ALL_LIBS += $(shell pkg-config opusfile --libs --static)
endif

ifeq ($(USE_LIBSNDFILE), true)
  SOURCES += decoding/decoders/libsndfile
  ALL_CFLAGS += -DUSE_LIBSNDFILE $(shell pkg-config sndfile --cflags)
  ALL_LIBS += $(shell pkg-config sndfile --libs --static)
endif

ifeq ($(USE_LIBOPENMPT), true)
  SOURCES += decoding/decoders/libopenmpt
  ALL_CFLAGS += -DUSE_LIBOPENMPT $(shell pkg-config libopenmpt --cflags)
  ALL_LIBS += $(shell pkg-config libopenmpt --libs --static)
endif

ifeq ($(USE_LIBXMPLITE), true)
  SOURCES += decoding/decoders/libxmp-lite
  ALL_CFLAGS += -DUSE_LIBXMPLITE

  ifeq ($(shell pkg-config libxmp-lite --exists && echo 1), 1)
    ALL_CFLAGS += $(shell pkg-config libxmp-lite --cflags)
    ALL_LIBS += $(shell pkg-config libxmp-lite --libs --static)
  else
    ALL_CFLAGS += -I$(CLOWNAUDIO_DIR)/decoding/decoders/libs/libxmp-lite/include/libxmp-lite
  endif
endif

ifeq ($(USE_SNES_SPC), true)
  SOURCES += decoding/decoders/snes_spc
  ALL_CFLAGS += -DUSE_SNES_SPC
  ALL_LIBS += -lstdc++
endif

ifeq ($(USE_PXTONE), true)
  SOURCES += decoding/decoders/pxtone decoding/decoders/pxtone_noise
  ALL_CFLAGS += -DUSE_PXTONE
  ALL_LIBS += -lstdc++
  # Apparently PxTone supports Vorbis-encoded samples
  ifeq ($(USE_LIBVORBIS), true)
    ALL_CXXFLAGS += -DpxINCLUDE_OGGVORBIS
  endif
endif

ifeq ($(BACKEND), miniaudio)
  SOURCES += playback/miniaudio
  ALL_CFLAGS += -DMINIAUDIO_ENABLE_DEVICE_IO
  ALL_LIBS += -ldl -lpthread -lm
else ifeq ($(BACKEND), SDL1)
  SOURCES += playback/sdl1
  ALL_CFLAGS += $(SDL1_CFLAGS)
  ALL_LIBS += $(SDL1_LIBS)
else ifeq ($(BACKEND), SDL2)
  SOURCES += playback/sdl2
  ALL_CFLAGS += $(SDL2_CFLAGS)
  ALL_LIBS += $(SDL2_LIBS)
else ifeq ($(BACKEND), Cubeb)
  SOURCES += playback/cubeb
  ALL_LIBS += -lcubeb
else ifeq ($(BACKEND), PortAudio)
  SOURCES += playback/portaudio
  ALL_CFLAGS += $(shell pkg-config portaudio-2.0 --cflags)
  ALL_LIBS += $(shell pkg-config portaudio-2.0 --libs --static)
endif

LIBXMPLITE_SOURCES = \
  src/virtual.c \
  src/format.c \
  src/period.c \
  src/player.c \
  src/read_event.c \
  src/dataio.c \
  src/lfo.c \
  src/scan.c \
  src/control.c \
  src/filter.c \
  src/effects.c \
  src/mixer.c \
  src/mix_all.c \
  src/load_helpers.c \
  src/load.c \
  src/hio.c \
  src/smix.c \
  src/memio.c \
  src/loaders/common.c \
  src/loaders/itsex.c \
  src/loaders/sample.c \
  src/loaders/xm_load.c \
  src/loaders/mod_load.c \
  src/loaders/s3m_load.c \
  src/loaders/it_load.c

SPC_SOURCES = \
  dsp.cpp \
  SNES_SPC.cpp \
  SNES_SPC_misc.cpp \
  SNES_SPC_state.cpp \
  spc.cpp \
  SPC_DSP.cpp \
  SPC_Filter.cpp

PXTONE_SOURCES = \
  pxtnDelay.cpp \
  pxtnDescriptor.cpp \
  pxtnError.cpp \
  pxtnEvelist.cpp \
  pxtnMaster.cpp \
  pxtnMem.cpp \
  pxtnOverDrive.cpp \
  pxtnPulse_Frequency.cpp \
  pxtnPulse_Noise.cpp \
  pxtnPulse_NoiseBuilder.cpp \
  pxtnPulse_Oggv.cpp \
  pxtnPulse_Oscillator.cpp \
  pxtnPulse_PCM.cpp \
  pxtnService.cpp \
  pxtnService_moo.cpp \
  pxtnText.cpp \
  pxtnUnit.cpp \
  pxtnWoice.cpp \
  pxtnWoice_io.cpp \
  pxtnWoicePTV.cpp \
  pxtoneNoise.cpp

OBJECTS += obj/test.o
OBJECTS += $(addprefix obj/clownaudio/, $(addsuffix .o, $(SOURCES)))
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

obj/test.o: test.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra -pedantic $< -o $@ -c

obj/clownaudio/%.o: $(CLOWNAUDIO_DIR)/%.c
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra -pedantic $< -o $@ -c

obj/clownaudio/%.o: $(CLOWNAUDIO_DIR)/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=c++11 -Wall -Wextra -pedantic $< -o $@ -c

obj/libxmp-lite/%.o: $(CLOWNAUDIO_DIR)/decoding/decoders/libs/libxmp-lite/%
	@mkdir -p $(@D)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra -pedantic -Wno-unused-parameter -Wno-sign-compare -Wno-maybe-uninitialized -I$(CLOWNAUDIO_DIR)/decoding/decoders/libs/libxmp-lite/src -DLIBXMP_CORE_PLAYER=1 -Dinline=__inline -D_USE_MATH_DEFINES=1 -DBUILDING_STATIC=1 $< -o $@ -c

obj/spc/%.o: $(CLOWNAUDIO_DIR)/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/%
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=c++98 $< -o $@ -c

obj/pxtone/%.o: $(CLOWNAUDIO_DIR)/decoding/decoders/libs/pxtone/%
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -std=c++11 -Wall -Wextra -pedantic $< -o $@ -c

test: $(OBJECTS)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra -pedantic $^ -o $@ $(ALL_LDFLAGS) $(ALL_LIBS)

include $(wildcard $(DEPENDENCIES))
