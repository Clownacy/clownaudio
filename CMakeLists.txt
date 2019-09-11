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
	"clownaudio/clownaudio.c"
	"clownaudio/clownaudio.h"
	"clownaudio/decoder.c"
	"clownaudio/decoder.h"
	"clownaudio/miniaudio.c"
	"clownaudio/miniaudio.h"
	"clownaudio/mixer.c"
	"clownaudio/mixer.h"
	"clownaudio/playback.h"
	"clownaudio/decoders/common.h"
	"clownaudio/decoders/memory_file.c"
	"clownaudio/decoders/memory_file.h"
	"clownaudio/decoders/misc_utilities.c"
	"clownaudio/decoders/misc_utilities.h"
	"clownaudio/decoders/predecode.c"
	"clownaudio/decoders/predecode.h"
	"clownaudio/decoders/split.c"
	"clownaudio/decoders/split.h"
)

if(LIBVORBIS)
	target_compile_definitions(test PRIVATE USE_LIBVORBIS)
	target_sources(test PRIVATE "clownaudio/decoders/libvorbis.c" "clownaudio/decoders/libvorbis.h")
endif()

if(TREMOR)
	target_compile_definitions(test PRIVATE USE_TREMOR)
	target_sources(test PRIVATE "clownaudio/decoders/tremor.c" "clownaudio/decoders/tremor.h")
endif()

if(STB_VORBIS)
	target_compile_definitions(test PRIVATE USE_STB_VORBIS)
	target_sources(test PRIVATE "clownaudio/decoders/stb_vorbis.c" "clownaudio/decoders/stb_vorbis.h" "clownaudio/decoders/libs/stb_vorbis.h")
endif()

if(LIBFLAC)
	target_compile_definitions(test PRIVATE USE_LIBFLAC)
	target_sources(test PRIVATE "clownaudio/decoders/libflac.c" "clownaudio/decoders/libflac.h")
endif()

if(DR_FLAC)
	target_compile_definitions(test PRIVATE USE_DR_FLAC)
	target_sources(test PRIVATE "clownaudio/decoders/dr_flac.c" "clownaudio/decoders/dr_flac.h" "clownaudio/decoders/libs/dr_flac.h")
endif()

if(DR_WAV)
	target_compile_definitions(test PRIVATE USE_DR_WAV)
	target_sources(test PRIVATE "clownaudio/decoders/dr_wav.c" "clownaudio/decoders/dr_wav.h" "clownaudio/decoders/libs/dr_wav.h")
endif()

if(LIBSNDFILE)
	target_compile_definitions(test PRIVATE USE_LIBSNDFILE)
	target_sources(test PRIVATE "clownaudio/decoders/libsndfile.c" "clownaudio/decoders/libsndfile.h")
endif()

if(LIBOPENMPT)
	target_compile_definitions(test PRIVATE USE_LIBOPENMPT)
	target_sources(test PRIVATE "clownaudio/decoders/libopenmpt.c" "clownaudio/decoders/libopenmpt.h")
endif()

if(LIBXMPLITE)
	target_compile_definitions(test PRIVATE USE_LIBXMPLITE)
	target_sources(test PRIVATE "clownaudio/decoders/libxmp-lite.c" "clownaudio/decoders/libxmp-lite.h")
endif()

if(SNES_SPC)
	target_compile_definitions(test PRIVATE USE_SNES_SPC)
	target_sources(test PRIVATE "clownaudio/decoders/snes_spc.c" "clownaudio/decoders/snes_spc.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_common.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_config.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_endian.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_source.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_misc.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_state.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/spc.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/spc.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_CPU.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.h"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.cpp"
		"clownaudio/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.h"
	)
endif()

if(PXTONE)
	target_compile_definitions(test PRIVATE USE_PXTONE)
	target_sources(test PRIVATE "clownaudio/decoders/pxtone.c" "clownaudio/decoders/pxtone.h"
		"clownaudio/decoders/libs/pxtone/pxtn.h"
		"clownaudio/decoders/libs/pxtone/pxtnDelay.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnDelay.h"
		"clownaudio/decoders/libs/pxtone/pxtnDescriptor.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnDescriptor.h"
		"clownaudio/decoders/libs/pxtone/pxtnError.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnError.h"
		"clownaudio/decoders/libs/pxtone/pxtnEvelist.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnEvelist.h"
		"clownaudio/decoders/libs/pxtone/pxtnMaster.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnMaster.h"
		"clownaudio/decoders/libs/pxtone/pxtnMax.h"
		"clownaudio/decoders/libs/pxtone/pxtnMem.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnMem.h"
		"clownaudio/decoders/libs/pxtone/pxtnOverDrive.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnOverDrive.h"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Frequency.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Frequency.h"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Noise.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Noise.h"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Oggv.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Oggv.h"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_Oscillator.h"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_PCM.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnPulse_PCM.h"
		"clownaudio/decoders/libs/pxtone/pxtnService.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnService.h"
		"clownaudio/decoders/libs/pxtone/pxtnService_moo.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnText.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnText.h"
		"clownaudio/decoders/libs/pxtone/pxtnUnit.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnUnit.h"
		"clownaudio/decoders/libs/pxtone/pxtnWoice.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnWoice.h"
		"clownaudio/decoders/libs/pxtone/pxtnWoice_io.cpp"
		"clownaudio/decoders/libs/pxtone/pxtnWoicePTV.cpp"
		"clownaudio/decoders/libs/pxtone/pxtoneNoise.cpp"
		"clownaudio/decoders/libs/pxtone/pxtoneNoise.h"
		"clownaudio/decoders/libs/pxtone/shim.cpp"
		"clownaudio/decoders/libs/pxtone/shim.h"
	)
endif()

if(BACKEND STREQUAL "miniaudio")
	target_sources(test PRIVATE "clownaudio/playback/miniaudio.c")
	target_compile_definitions(test PRIVATE MINIAUDIO_ENABLE_DEVICE_IO)
	target_link_libraries(test PRIVATE ${CMAKE_DL_LIBS} m pthread)
elseif(BACKEND STREQUAL "SDL1")
	target_sources(test PRIVATE "clownaudio/playback/sdl1.c")
elseif(BACKEND STREQUAL "SDL2")
	target_sources(test PRIVATE "clownaudio/playback/sdl2.c")
elseif(BACKEND STREQUAL "Cubeb")
	target_sources(test PRIVATE "clownaudio/playback/cubeb.c")
elseif(BACKEND STREQUAL "PortAudio")
	target_sources(test PRIVATE "clownaudio/playback/portaudio.c")
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
