cmake_minimum_required(VERSION 3.14.6)

#option(CLOWNAUDIO_LIBVORBIS "Enable the libvorbis decoder backend" OFF)
option(CLOWNAUDIO_STB_VORBIS "Enable the stb_vorbis decoder backend" OFF)
#option(CLOWNAUDIO_LIBFLAC "Enable the libFLAC decoder backend" OFF)
option(CLOWNAUDIO_DR_FLAC "Enable the dr_flac decoder backend" OFF)
option(CLOWNAUDIO_DR_WAV "Enable the dr_wav decoder backend" OFF)
#option(CLOWNAUDIO_LIBOPUS "Enable the libopus decoder backend" OFF)
#option(CLOWNAUDIO_LIBSNDFILE "Enable the libsndfile decoder backend" OFF)
#option(CLOWNAUDIO_LIBOPENMPT "Enable the libopenmpt decoder backend" OFF)
option(CLOWNAUDIO_LIBXMPLITE "Enable the libxmp-lite decoder backend" OFF)
option(CLOWNAUDIO_SNES_SPC "Enable the snes_spc decoder backend" OFF)
option(CLOWNAUDIO_PXTONE "Enable the PxTone decoder backend" OFF)
#set(CLOWNAUDIO_BACKEND "miniaudio" CACHE STRING "Which playback backend to use: supported options are 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', and 'PortAudio'")
set(CLOWNAUDIO_BACKEND "miniaudio" CACHE STRING "Which playback backend to use: supported options are 'miniaudio', 'SDL1', and 'SDL2'")

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_LIST_DIR}/cmake"
)

project(clownaudio LANGUAGES C CXX)

set(SOURCES
	"clownaudio.c"
	"clownaudio.h"
	"miniaudio.c"
	"miniaudio.h"
	"mixer.c"
	"mixer.h"
	"playback/playback.h"
	"decoding/decoder_selector.c"
	"decoding/decoder_selector.h"
	"decoding/predecoder.c"
	"decoding/predecoder.h"
	"decoding/resampled_decoder.c"
	"decoding/resampled_decoder.h"
	"decoding/split_decoder.c"
	"decoding/split_decoder.h"
	"decoding/decoders/common.h"
	"decoding/decoders/memory_stream.c"
	"decoding/decoders/memory_stream.h"
)

if(BUILD_SHARED_LIBS)
	add_library(clownaudio SHARED ${SOURCES})

	if(WIN32)
		target_compile_definitions(clownaudio PRIVATE BUILDING_DLL)
	endif()
else()
	add_library(clownaudio STATIC ${SOURCES})
endif()

set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "clownaudio.h")

if(CLOWNAUDIO_LIBVORBIS)
	target_compile_definitions(clownaudio PRIVATE USE_LIBVORBIS)
	target_sources(clownaudio PRIVATE "decoding/decoders/libvorbis.c" "decoding/decoders/libvorbis.h")
endif()

if(CLOWNAUDIO_STB_VORBIS)
	target_compile_definitions(clownaudio PRIVATE USE_STB_VORBIS)
	target_sources(clownaudio PRIVATE "decoding/decoders/stb_vorbis.c" "decoding/decoders/stb_vorbis.h")
endif()

if(CLOWNAUDIO_LIBFLAC)
	target_compile_definitions(clownaudio PRIVATE USE_LIBFLAC)
	target_sources(clownaudio PRIVATE "decoding/decoders/libflac.c" "decoding/decoders/libflac.h")
endif()

if(CLOWNAUDIO_DR_FLAC)
	target_compile_definitions(clownaudio PRIVATE USE_DR_FLAC)
	target_sources(clownaudio PRIVATE "decoding/decoders/dr_flac.c" "decoding/decoders/dr_flac.h" "decoding/decoders/libs/dr_flac.h")
endif()

if(CLOWNAUDIO_DR_WAV)
	target_compile_definitions(clownaudio PRIVATE USE_DR_WAV)
	target_sources(clownaudio PRIVATE "decoding/decoders/dr_wav.c" "decoding/decoders/dr_wav.h" "decoding/decoders/libs/dr_wav.h")
endif()

if(CLOWNAUDIO_LIBOPUS)
	target_compile_definitions(clownaudio PRIVATE USE_LIBOPUS)
	target_sources(clownaudio PRIVATE "decoding/decoders/libopus.c" "decoding/decoders/libopus.h")
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	target_compile_definitions(clownaudio PRIVATE USE_LIBSNDFILE)
	target_sources(clownaudio PRIVATE "decoding/decoders/libsndfile.c" "decoding/decoders/libsndfile.h")
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	target_compile_definitions(clownaudio PRIVATE USE_LIBOPENMPT)
	target_sources(clownaudio PRIVATE "decoding/decoders/libopenmpt.c" "decoding/decoders/libopenmpt.h")
endif()

if(CLOWNAUDIO_LIBXMPLITE)
	target_compile_definitions(clownaudio PRIVATE USE_LIBXMPLITE)
	target_sources(clownaudio PRIVATE "decoding/decoders/libxmp-lite.c" "decoding/decoders/libxmp-lite.h")
endif()

if(CLOWNAUDIO_SNES_SPC)
	target_compile_definitions(clownaudio PRIVATE USE_SNES_SPC)
	target_sources(clownaudio PRIVATE
		"decoding/decoders/snes_spc.c" "decoding/decoders/snes_spc.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_common.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_config.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_endian.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_source.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_misc.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_state.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/spc.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/spc.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_CPU.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.h"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.cpp"
		"decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.h"
	)
endif()

if(CLOWNAUDIO_PXTONE)
	target_compile_definitions(clownaudio PRIVATE USE_PXTONE)
	target_sources(clownaudio PRIVATE
		"decoding/decoders/pxtone.cpp" "decoding/decoders/pxtone.h"
		"decoding/decoders/pxtone_noise.cpp" "decoding/decoders/pxtone_noise.h"
		"decoding/decoders/libs/pxtone/pxtn.h"
		"decoding/decoders/libs/pxtone/pxtnDelay.cpp"
		"decoding/decoders/libs/pxtone/pxtnDelay.h"
		"decoding/decoders/libs/pxtone/pxtnDescriptor.cpp"
		"decoding/decoders/libs/pxtone/pxtnDescriptor.h"
		"decoding/decoders/libs/pxtone/pxtnError.cpp"
		"decoding/decoders/libs/pxtone/pxtnError.h"
		"decoding/decoders/libs/pxtone/pxtnEvelist.cpp"
		"decoding/decoders/libs/pxtone/pxtnEvelist.h"
		"decoding/decoders/libs/pxtone/pxtnMaster.cpp"
		"decoding/decoders/libs/pxtone/pxtnMaster.h"
		"decoding/decoders/libs/pxtone/pxtnMax.h"
		"decoding/decoders/libs/pxtone/pxtnMem.cpp"
		"decoding/decoders/libs/pxtone/pxtnMem.h"
		"decoding/decoders/libs/pxtone/pxtnOverDrive.cpp"
		"decoding/decoders/libs/pxtone/pxtnOverDrive.h"
		"decoding/decoders/libs/pxtone/pxtnPulse_Frequency.cpp"
		"decoding/decoders/libs/pxtone/pxtnPulse_Frequency.h"
		"decoding/decoders/libs/pxtone/pxtnPulse_Noise.cpp"
		"decoding/decoders/libs/pxtone/pxtnPulse_Noise.h"
		"decoding/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp"
		"decoding/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h"
		"decoding/decoders/libs/pxtone/pxtnPulse_Oggv.cpp"
		"decoding/decoders/libs/pxtone/pxtnPulse_Oggv.h"
		"decoding/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp"
		"decoding/decoders/libs/pxtone/pxtnPulse_Oscillator.h"
		"decoding/decoders/libs/pxtone/pxtnPulse_PCM.cpp"
		"decoding/decoders/libs/pxtone/pxtnPulse_PCM.h"
		"decoding/decoders/libs/pxtone/pxtnService.cpp"
		"decoding/decoders/libs/pxtone/pxtnService.h"
		"decoding/decoders/libs/pxtone/pxtnService_moo.cpp"
		"decoding/decoders/libs/pxtone/pxtnText.cpp"
		"decoding/decoders/libs/pxtone/pxtnText.h"
		"decoding/decoders/libs/pxtone/pxtnUnit.cpp"
		"decoding/decoders/libs/pxtone/pxtnUnit.h"
		"decoding/decoders/libs/pxtone/pxtnWoice.cpp"
		"decoding/decoders/libs/pxtone/pxtnWoice.h"
		"decoding/decoders/libs/pxtone/pxtnWoice_io.cpp"
		"decoding/decoders/libs/pxtone/pxtnWoicePTV.cpp"
		"decoding/decoders/libs/pxtone/pxtoneNoise.cpp"
		"decoding/decoders/libs/pxtone/pxtoneNoise.h"
	)
endif()

if(CLOWNAUDIO_BACKEND STREQUAL "miniaudio")
	target_sources(clownaudio PRIVATE "playback/miniaudio.c")
	target_compile_definitions(clownaudio PRIVATE MINIAUDIO_ENABLE_DEVICE_IO)

	include(CheckLibraryExists)

	check_library_exists(m pow "" LIBM)
	if(LIBM)
		target_link_libraries(clownaudio PRIVATE m)
	endif()

	check_library_exists(pthread pthread_create "" LIBPTHREAD)
	if(LIBPTHREAD)
		target_link_libraries(clownaudio PRIVATE pthread)
	endif()

	target_link_libraries(clownaudio PRIVATE ${CMAKE_DL_LIBS})
elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL1")
	target_sources(clownaudio PRIVATE "playback/sdl1.c")
elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
	target_sources(clownaudio PRIVATE "playback/sdl2.c")
elseif(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
	target_sources(clownaudio PRIVATE "playback/cubeb.c")
elseif(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
	target_sources(clownaudio PRIVATE "playback/portaudio.c")
else()
	message(FATAL_ERROR "Invalid BACKEND selected")
endif()

set_target_properties(clownaudio PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS OFF
	CXX_STANDARD 11
	CXX_EXTENSIONS OFF
)

# MSVC tweak
if(MSVC)
	target_compile_definitions(clownaudio PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()

# Find libraries
if(CLOWNAUDIO_BACKEND STREQUAL "SDL1")
	find_package(SDL REQUIRED)

	if(SDL_FOUND)
		target_include_directories(clownaudio PRIVATE ${SDL_INCLUDE_DIR})
		target_link_libraries(clownaudio PRIVATE ${SDL_LIBRARY})
	endif()
endif()

if(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
	find_package(SDL2 REQUIRED)

	if(TARGET SDL2::SDL2)
		# CMake-generated config (Arch, vcpkg, Raspbian)
		target_link_libraries(clownaudio PRIVATE SDL2::SDL2 SDL2::SDL2main)
	elseif(SDL2_FOUND)
		# Autotools-generated config (MSYS2)
		target_include_directories(clownaudio PRIVATE ${SDL2_INCLUDE_DIRS})
		target_link_libraries(clownaudio PRIVATE ${SDL2_LIBRARIES})
	endif()
endif()

if(CLOWNAUDIO_LIBXMPLITE)
	find_package(LibXMPLite)

	if(libxmp-lite_FOUND)
		message(STATUS "Using system libxmp-lite")
		target_link_libraries(clownaudio PRIVATE libxmp-lite)
	else()
		# Compile it ourselves
		message(STATUS "Using local libxmp-lite")
		add_subdirectory("decoding/decoders/libs/libxmp-lite" EXCLUDE_FROM_ALL)
		target_link_libraries(clownaudio PRIVATE libxmp-lite)
	endif()
endif()
