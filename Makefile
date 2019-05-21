USE_LIBVORBIS = false
USE_TREMOR = false
USE_STB_VORBIS = true
USE_LIBFLAC = false
USE_DR_FLAC = true
USE_DR_WAV = true
USE_LIBSNDFILE = false
USE_LIBOPENMPT = false
USE_LIBXMPLITE = false
USE_SNES_SPC = true
USE_PXTONE = true
# Can be 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', or 'PortAudio'
BACKEND = miniaudio

CFLAGS = -O2 -s -flto
ALL_CFLAGS = -std=c11 -MMD -MP -MF $@.d $(CFLAGS)

CXXFLAGS = -O2 -s -flto
ALL_CXXFLAGS = -std=c++11 -MMD -MP -MF $@.d $(CXXFLAGS)

LDFLAGS =
ALL_LDFLAGS = $(LDFLAGS)

LIBS =
ALL_LIBS = $(LIBS)

SDL1_CFLAGS = `pkg-config sdl --cflags`
SDL1_LIBS = `pkg-config sdl --libs --static`

SDL2_CFLAGS = `pkg-config sdl2 --cflags`
SDL2_LIBS = `pkg-config sdl2 --libs --static`

SOURCES = \
	main \
	audio_lib/audio_lib \
	audio_lib/mixer \
	audio_lib/decoder \
	audio_lib/miniaudio \
	audio_lib/decoders/split \
	audio_lib/decoders/predecode \
	audio_lib/decoders/memory_file \
	audio_lib/decoders/misc_utilities

ifeq ($(USE_LIBVORBIS), true)
SOURCES += audio_lib/decoders/libvorbis
ALL_CFLAGS += -DUSE_LIBVORBIS `pkg-config vorbisfile --cflags`
ALL_LIBS += `pkg-config vorbisfile --libs --static`
endif

ifeq ($(USE_TREMOR), true)
SOURCES += audio_lib/decoders/tremor
ALL_CFLAGS += -DUSE_TREMOR `pkg-config vorbisidec --cflags`
ALL_LIBS += `pkg-config vorbisidec --libs --static`
endif

ifeq ($(USE_STB_VORBIS), true)
SOURCES += audio_lib/decoders/stb_vorbis
ALL_CFLAGS += -DUSE_STB_VORBIS
endif

ifeq ($(USE_LIBFLAC), true)
SOURCES += audio_lib/decoders/libflac
ALL_CFLAGS += -DUSE_LIBFLAC `pkg-config flac --cflags`
ALL_LIBS += `pkg-config flac --libs --static`
endif

ifeq ($(USE_DR_FLAC), true)
SOURCES += audio_lib/decoders/dr_flac
ALL_CFLAGS += -DUSE_DR_FLAC
endif

ifeq ($(USE_DR_WAV), true)
SOURCES += audio_lib/decoders/dr_wav
ALL_CFLAGS += -DUSE_DR_WAV
endif

ifeq ($(USE_LIBSNDFILE), true)
SOURCES += audio_lib/decoders/libsndfile
ALL_CFLAGS += -DUSE_LIBSNDFILE `pkg-config sndfile --cflags`
ALL_LIBS += `pkg-config sndfile --libs --static`
endif

ifeq ($(USE_LIBOPENMPT), true)
SOURCES += audio_lib/decoders/libopenmpt
ALL_CFLAGS += -DUSE_LIBOPENMPT `pkg-config libopenmpt --cflags`
ALL_LIBS += `pkg-config libopenmpt --libs --static`
endif

ifeq ($(USE_LIBXMPLITE), true)
SOURCES += audio_lib/decoders/libxmp-lite
ALL_CFLAGS += -DUSE_LIBXMPLITE `pkg-config libxmp-lite --cflags`
ALL_LIBS += `pkg-config libxmp-lite --libs --static`
endif

ifeq ($(USE_SNES_SPC), true)
SOURCES += audio_lib/decoders/snes_spc
ALL_CFLAGS += -DUSE_SNES_SPC
ALL_LIBS += -lstdc++
endif

ifeq ($(USE_PXTONE), true)
SOURCES += audio_lib/decoders/pxtone
ALL_CFLAGS += -DUSE_PXTONE
ALL_LIBS += -lstdc++
endif

ifeq ($(BACKEND), miniaudio)
SOURCES += audio_lib/playback/miniaudio
ALL_CFLAGS += -DMINIAUDIO_ENABLE_DEVICE_IO
ALL_LIBS += -ldl -lpthread -lm
else ifeq ($(BACKEND), SDL1)
SOURCES += audio_lib/playback/sdl1
ALL_CFLAGS += $(SDL1_CFLAGS)
ALL_LIBS += $(SDL1_LIBS)
else ifeq ($(BACKEND), SDL2)
SOURCES += audio_lib/playback/sdl2
ALL_CFLAGS += $(SDL2_CFLAGS)
ALL_LIBS += $(SDL2_LIBS)
else ifeq ($(BACKEND), Cubeb)
SOURCES += audio_lib/playback/cubeb
ALL_LIBS += -lcubeb -lole32
else ifeq ($(BACKEND), PortAudio)
SOURCES += audio_lib/playback/portaudio
ALL_CFLAGS += `pkg-config portaudio-2.0 --cflags`
ALL_LIBS += `pkg-config portaudio-2.0 --libs --static`
endif

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
	pxtoneNoise \
	shim

OBJECTS += $(addprefix obj/main/, $(addsuffix .o, $(SOURCES)))
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

obj/spc/%.o: audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) $< -o $@ -c

obj/pxtone/%.o: audio_lib/decoders/libs/pxtone/%.cpp
	@mkdir -p $(@D)
	@$(CXX) $(ALL_CXXFLAGS) -Wall -Wextra -Wno-switch -Wno-tautological-compare -Wno-sign-compare -Wno-unused-parameter -Wno-unused-value -Wno-unused-variable -Wno-missing-field-initializers -Wno-misleading-indentation -Wno-strict-aliasing $< -o $@ -c

test: $(OBJECTS)
	@$(CC) $(ALL_CFLAGS) -Wall -Wextra $^ -o $@ $(ALL_LDFLAGS) $(ALL_LIBS)

include $(wildcard $(DEPENDENCIES))
