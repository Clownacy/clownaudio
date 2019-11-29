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
set(BACKEND "miniaudio" CACHE STRING "Which playback backend to use: supported options are 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', and 'PortAudio'")

project(clownaudio LANGUAGES C CXX)

add_executable(test
	"test.c"
	"clownaudio/clownaudio.c"
	"clownaudio/clownaudio.h"
	"clownaudio/miniaudio.c"
	"clownaudio/miniaudio.h"
	"clownaudio/mixer.c"
	"clownaudio/mixer.h"
	"clownaudio/playback/playback.h"
	"clownaudio/decoder/decoder.c"
	"clownaudio/decoder/decoder.h"
	"clownaudio/decoder/predecoder.c"
	"clownaudio/decoder/predecoder.h"
	"clownaudio/decoder/resampled_decoder.c"
	"clownaudio/decoder/resampled_decoder.h"
	"clownaudio/decoder/split_decoder.c"
	"clownaudio/decoder/split_decoder.h"
	"clownaudio/decoder/backends/common.h"
	"clownaudio/decoder/backends/memory_stream.c"
	"clownaudio/decoder/backends/memory_stream.h"
)

if(LIBVORBIS)
	target_compile_definitions(test PRIVATE USE_LIBVORBIS)
	target_sources(test PRIVATE "clownaudio/decoder/backends/libvorbis.c" "clownaudio/decoder/backends/libvorbis.h")
endif()

if(TREMOR)
	target_compile_definitions(test PRIVATE USE_TREMOR)
	target_sources(test PRIVATE "clownaudio/decoder/backends/tremor.c" "clownaudio/decoder/backends/tremor.h")
endif()

if(STB_VORBIS)
	target_compile_definitions(test PRIVATE USE_STB_VORBIS)
	target_sources(test PRIVATE "clownaudio/decoder/backends/stb_vorbis.c" "clownaudio/decoder/backends/stb_vorbis.h")
endif()

if(LIBFLAC)
	target_compile_definitions(test PRIVATE USE_LIBFLAC)
	target_sources(test PRIVATE "clownaudio/decoder/backends/libflac.c" "clownaudio/decoder/backends/libflac.h")
endif()

if(DR_FLAC)
	target_compile_definitions(test PRIVATE USE_DR_FLAC)
	target_sources(test PRIVATE "clownaudio/decoder/backends/dr_flac.c" "clownaudio/decoder/backends/dr_flac.h" "clownaudio/decoder/backends/libs/dr_flac.h")
endif()

if(DR_WAV)
	target_compile_definitions(test PRIVATE USE_DR_WAV)
	target_sources(test PRIVATE "clownaudio/decoder/backends/dr_wav.c" "clownaudio/decoder/backends/dr_wav.h" "clownaudio/decoder/backends/libs/dr_wav.h")
endif()

if(LIBSNDFILE)
	target_compile_definitions(test PRIVATE USE_LIBSNDFILE)
	target_sources(test PRIVATE "clownaudio/decoder/backends/libsndfile.c" "clownaudio/decoder/backends/libsndfile.h")
endif()

if(LIBOPENMPT)
	target_compile_definitions(test PRIVATE USE_LIBOPENMPT)
	target_sources(test PRIVATE "clownaudio/decoder/backends/libopenmpt.c" "clownaudio/decoder/backends/libopenmpt.h")
endif()

if(LIBXMPLITE)
	target_compile_definitions(test PRIVATE USE_LIBXMPLITE)
	target_sources(test PRIVATE "clownaudio/decoder/backends/libxmp-lite.c" "clownaudio/decoder/backends/libxmp-lite.h")
endif()

if(SNES_SPC)
	target_compile_definitions(test PRIVATE USE_SNES_SPC)
	target_sources(test PRIVATE "clownaudio/decoder/backends/snes_spc.c" "clownaudio/decoder/backends/snes_spc.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/blargg_common.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/blargg_config.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/blargg_endian.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/blargg_source.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/dsp.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/dsp.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_misc.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_state.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/spc.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/spc.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SPC_CPU.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.h"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.cpp"
		"clownaudio/decoder/backends/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.h"
	)
endif()

if(PXTONE)
	target_compile_definitions(test PRIVATE USE_PXTONE)
	target_sources(test PRIVATE "clownaudio/decoder/backends/pxtone.cpp" "clownaudio/decoder/backends/pxtone.h"
		"clownaudio/decoder/backends/pxtone_noise.cpp" "clownaudio/decoder/backends/pxtone_noise.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtn.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnDelay.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnDelay.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnDescriptor.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnDescriptor.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnError.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnError.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnEvelist.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnEvelist.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnMaster.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnMaster.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnMax.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnMem.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnMem.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnOverDrive.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnOverDrive.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Frequency.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Frequency.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Noise.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Noise.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_NoiseBuilder.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_NoiseBuilder.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Oggv.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Oggv.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Oscillator.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_Oscillator.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_PCM.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnPulse_PCM.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnService.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnService.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnService_moo.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnText.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnText.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnUnit.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnUnit.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnWoice.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnWoice.h"
		"clownaudio/decoder/backends/libs/pxtone/pxtnWoice_io.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtnWoicePTV.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtoneNoise.cpp"
		"clownaudio/decoder/backends/libs/pxtone/pxtoneNoise.h"
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
