cmake_minimum_required(VERSION 3.12)

if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
	cmake_policy(SET CMP0069 NEW)
endif()

option(LIBVORBIS "Enable the libvorbis decoder backend" OFF)
option(TREMOR "Enable the Tremor decoder backend" OFF)
option(STB_VORBIS "Enable the stb_vorbis decoder backend" OFF)
option(LIBFLAC "Enable the libFLAC decoder backend" OFF)
option(DR_FLAC "Enable the dr_flac decoder backend" OFF)
option(DR_WAV "Enable the dr_wav decoder backend" OFF)
option(LIBSNDFILE "Enable the libsndfile decoder backend" OFF)
option(LIBOPENMPT "Enable the libopenmpt decoder backend" OFF)
option(LIBXMPLITE "Enable the libxmp-lite decoder backend" OFF)
option(SNES_SPC "Enable the snes_spc decoder backend" OFF)
option(PXTONE "Enable the PxTone decoder backend" OFF)

if(NOT BACKEND)
	set(BACKEND "miniaudio")
endif()

project(clownaudio LANGUAGES C CXX)

add_executable(test
	"main.c"
	"audio_lib/audio_lib.c"
	"audio_lib/audio_lib.h"
	"audio_lib/decoder.c"
	"audio_lib/decoder.h"
	"audio_lib/miniaudio.c"
	"audio_lib/miniaudio.h"
	"audio_lib/mixer.c"
	"audio_lib/mixer.h"
	"audio_lib/playback.h"
	"audio_lib/decoders/common.h"
	"audio_lib/decoders/memory_file.c"
	"audio_lib/decoders/memory_file.h"
	"audio_lib/decoders/misc_utilities.c"
	"audio_lib/decoders/misc_utilities.h"
	"audio_lib/decoders/predecode.c"
	"audio_lib/decoders/predecode.h"
	"audio_lib/decoders/split.c"
	"audio_lib/decoders/split.h"
)

if(LIBVORBIS)
	target_compile_definitions(test PRIVATE USE_LIBVORBIS)
	target_sources(test PRIVATE "audio_lib/decoders/libvorbis.c" "audio_lib/decoders/libvorbis.h")
endif()

if(TREMOR)
	target_compile_definitions(test PRIVATE USE_TREMOR)
	target_sources(test PRIVATE "audio_lib/decoders/tremor.c" "audio_lib/decoders/tremor.h")
endif()

if(STB_VORBIS)
	target_compile_definitions(test PRIVATE USE_STB_VORBIS)
	target_sources(test PRIVATE "audio_lib/decoders/stb_vorbis.c" "audio_lib/decoders/stb_vorbis.h" "audio_lib/decoders/libs/stb_vorbis.h")
endif()

if(LIBFLAC)
	target_compile_definitions(test PRIVATE USE_LIBFLAC)
	target_sources(test PRIVATE "audio_lib/decoders/libflac.c" "audio_lib/decoders/libflac.h")
endif()

if(DR_FLAC)
	target_compile_definitions(test PRIVATE USE_DR_FLAC)
	target_sources(test PRIVATE "audio_lib/decoders/dr_flac.c" "audio_lib/decoders/dr_flac.h" "audio_lib/decoders/libs/dr_flac.h")
endif()

if(DR_WAV)
	target_compile_definitions(test PRIVATE USE_DR_WAV)
	target_sources(test PRIVATE "audio_lib/decoders/dr_wav.c" "audio_lib/decoders/dr_wav.h" "audio_lib/decoders/libs/dr_wav.h")
endif()

if(LIBSNDFILE)
	target_compile_definitions(test PRIVATE USE_LIBSNDFILE)
	target_sources(test PRIVATE "audio_lib/decoders/libsndfile.c" "audio_lib/decoders/libsndfile.h")
endif()

if(LIBOPENMPT)
	target_compile_definitions(test PRIVATE USE_LIBOPENMPT)
	target_sources(test PRIVATE "audio_lib/decoders/libopenmpt.c" "audio_lib/decoders/libopenmpt.h")
endif()

if(LIBXMPLITE)
	target_compile_definitions(test PRIVATE USE_LIBXMPLITE)
	target_sources(test PRIVATE "audio_lib/decoders/libxmp-lite.c" "audio_lib/decoders/libxmp-lite.h")
endif()

if(SNES_SPC)
	target_compile_definitions(test PRIVATE USE_SNES_SPC)
	target_sources(test PRIVATE "audio_lib/decoders/snes_spc.c" "audio_lib/decoders/snes_spc.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_common.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_config.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_endian.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_source.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_misc.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_state.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/spc.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/spc.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_CPU.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.h"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.cpp"
		"audio_lib/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.h"
	)
endif()

if(PXTONE)
	target_compile_definitions(test PRIVATE USE_PXTONE)
	target_sources(test PRIVATE "audio_lib/decoders/pxtone.c" "audio_lib/decoders/pxtone.h"
		"audio_lib/decoders/libs/pxtone/pxtn.h"
		"audio_lib/decoders/libs/pxtone/pxtnDelay.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnDelay.h"
		"audio_lib/decoders/libs/pxtone/pxtnDescriptor.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnDescriptor.h"
		"audio_lib/decoders/libs/pxtone/pxtnError.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnError.h"
		"audio_lib/decoders/libs/pxtone/pxtnEvelist.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnEvelist.h"
		"audio_lib/decoders/libs/pxtone/pxtnMaster.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnMaster.h"
		"audio_lib/decoders/libs/pxtone/pxtnMax.h"
		"audio_lib/decoders/libs/pxtone/pxtnMem.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnMem.h"
		"audio_lib/decoders/libs/pxtone/pxtnOverDrive.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnOverDrive.h"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Frequency.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Frequency.h"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Noise.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Noise.h"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Oggv.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Oggv.h"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_Oscillator.h"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_PCM.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnPulse_PCM.h"
		"audio_lib/decoders/libs/pxtone/pxtnService.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnService.h"
		"audio_lib/decoders/libs/pxtone/pxtnService_moo.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnText.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnText.h"
		"audio_lib/decoders/libs/pxtone/pxtnUnit.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnUnit.h"
		"audio_lib/decoders/libs/pxtone/pxtnWoice.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnWoice.h"
		"audio_lib/decoders/libs/pxtone/pxtnWoice_io.cpp"
		"audio_lib/decoders/libs/pxtone/pxtnWoicePTV.cpp"
		"audio_lib/decoders/libs/pxtone/pxtoneNoise.cpp"
		"audio_lib/decoders/libs/pxtone/pxtoneNoise.h"
		"audio_lib/decoders/libs/pxtone/shim.cpp"
		"audio_lib/decoders/libs/pxtone/shim.h"
	)
endif()

if(BACKEND STREQUAL "miniaudio")
	target_sources(test PRIVATE "audio_lib/playback/miniaudio.c")
	target_compile_definitions(test PRIVATE MINIAUDIO_ENABLE_DEVICE_IO)
	#target_link_libraries(test PRIVATE $(CMAKE_DL_LIBS) m pthread)
elseif(BACKEND STREQUAL "SDL1")
	target_sources(test PRIVATE "audio_lib/playback/sdl1.c")
elseif(BACKEND STREQUAL "SDL2")
	target_sources(test PRIVATE "audio_lib/playback/sdl2.c")
elseif(BACKEND STREQUAL "Cubeb")
	target_sources(test PRIVATE "audio_lib/playback/cubeb.c")
elseif(BACKEND STREQUAL "PortAudio")
	target_sources(test PRIVATE "audio_lib/playback/portaudio.c")
else()
	message(FATAL_ERROR "Invalid BACKEND selected")
endif()

set_target_properties(test PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS OFF
	CXX_STANDARD 11
	CXX_EXTENSIONS OFF
)

# MSVC tweak
if(MSVC)
	target_compile_definitions(test PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()

# Enable link-time optimisation if available
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
	if((${CMAKE_VERSION} VERSION_EQUAL 3.9) OR (${CMAKE_VERSION} VERSION_GREATER 3.9))
		include(CheckIPOSupported)
		check_ipo_supported(RESULT result)
		if(result)
			set_target_properties(test PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
		endif()
	endif()
endif()

# Find libraries
if(BACKEND STREQUAL "SDL1")
	find_package(SDL REQUIRED)

	if(SDL_FOUND)
		target_include_directories(test PRIVATE ${SDL_INCLUDE_DIR})
		target_link_libraries(test ${SDL_LIBRARY})
	endif()
endif()

if(BACKEND STREQUAL "SDL2")
	find_package(SDL2 REQUIRED)

	if(TARGET SDL2::SDL2)
		# CMake-generated config (Arch, vcpkg, Raspbian)
		target_link_libraries(test SDL2::SDL2 SDL2::SDL2main)
	elseif(SDL2_FOUND)
		# Autotools-generated config (MSYS2)
		target_include_directories(test PRIVATE ${SDL2_INCLUDE_DIRS})
		target_link_libraries(test ${SDL2_LIBRARIES})
	endif()
endif()
