cmake_minimum_required(VERSION 3.12)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(CLOWNAUDIO_CPP "Compile the library as C++ instead of C, for compilers that don't support C99 but do support C++98" OFF)
option(CLOWNAUDIO_LIBVORBIS "Enable the libvorbis decoder backend" OFF)
option(CLOWNAUDIO_STB_VORBIS "Enable the stb_vorbis decoder backend" ON)
option(CLOWNAUDIO_DR_MP3 "Enable the dr_mp3 decoder backend" ON)
option(CLOWNAUDIO_LIBOPUS "Enable the libopus decoder backend" OFF)
option(CLOWNAUDIO_LIBFLAC "Enable the libFLAC decoder backend" OFF)
option(CLOWNAUDIO_DR_FLAC "Enable the dr_flac decoder backend" ON)
option(CLOWNAUDIO_DR_WAV "Enable the dr_wav decoder backend" ON)
option(CLOWNAUDIO_LIBSNDFILE "Enable the libsndfile decoder backend" OFF)
option(CLOWNAUDIO_LIBOPENMPT "Enable the libopenmpt decoder backend" OFF)
option(CLOWNAUDIO_LIBXMP "Enable the libxmp decoder backend" OFF)
option(CLOWNAUDIO_LIBXMPLITE "Enable the libxmp-lite decoder backend" OFF)
option(CLOWNAUDIO_PXTONE "Enable the PxTone decoder backend" OFF)
option(CLOWNAUDIO_SNES_SPC "Enable the snes_spc decoder backend" OFF)
option(CLOWNAUDIO_MIXER_ONLY "Disables playback capabilities" OFF)
if(NOT CLOWNAUDIO_MIXER_ONLY)
	set(CLOWNAUDIO_BACKEND "miniaudio" CACHE STRING "Which playback backend to use: supported options are 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', and 'PortAudio'")
endif()

# Figure out if we need a C compiler, a C++ compiler, or both
if(CLOWNAUDIO_CPP)
	set(LANGUAGES CXX)
else()
	if(CLOWNAUDIO_PXTONE OR CLOWNAUDIO_SNES_SPC)
		set(LANGUAGES C CXX)
	else()
		set(LANGUAGES C)
	endif()
endif()

project(clownaudio VERSION 0.4 DESCRIPTION "Stereo audio engine supporting various formats" LANGUAGES ${LANGUAGES})

add_library(clownaudio)

list(APPEND C_AND_CPP_SOURCES
	"src/miniaudio.c"
	"src/mixer.c"
	"src/decoding/decoder_selector.c"
	"src/decoding/predecoder.c"
	"src/decoding/resampled_decoder.c"
	"src/decoding/split_decoder.c"
	"src/decoding/decoders/memory_stream.c"
)

target_sources(clownaudio PRIVATE
	"include/clownaudio/mixer.h"
	"src/miniaudio.h"
	"src/decoding/decoder_selector.h"
	"src/decoding/predecoder.h"
	"src/decoding/resampled_decoder.h"
	"src/decoding/split_decoder.h"
	"src/decoding/decoders/common.h"
	"src/decoding/decoders/memory_stream.h"
)

if(NOT CLOWNAUDIO_MIXER_ONLY)
	list(APPEND C_AND_CPP_SOURCES "src/clownaudio.c")

	target_sources(clownaudio PRIVATE
		"include/clownaudio/clownaudio.h"
		"include/clownaudio/playback.h"
	)
endif()


#####################
# Decoding backends #
#####################

if(CLOWNAUDIO_LIBVORBIS)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_LIBVORBIS)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/libvorbis.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libvorbis.h")
endif()

if(CLOWNAUDIO_STB_VORBIS)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_STB_VORBIS)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/stb_vorbis.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/stb_vorbis.h")

	include(CheckLibraryExists)

	check_library_exists(m exp "" LIBM)
	if(LIBM)
		target_link_libraries(clownaudio PRIVATE m)
		list(APPEND STATIC_LIBS m)
	endif()
endif()

if(CLOWNAUDIO_DR_MP3)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_DR_MP3)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/dr_mp3.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/dr_mp3.h")
endif()

if(CLOWNAUDIO_LIBOPUS)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_LIBOPUS)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/libopus.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libopus.h")
endif()

if(CLOWNAUDIO_LIBFLAC)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_LIBFLAC)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/libflac.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libflac.h")
endif()

if(CLOWNAUDIO_DR_FLAC)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_DR_FLAC)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/dr_flac.c")
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/dr_flac.h"
		"src/decoding/decoders/libs/dr_flac.h"
	)
endif()

if(CLOWNAUDIO_DR_WAV)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_DR_WAV)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/dr_wav.c")
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/dr_wav.h"
		"src/decoding/decoders/libs/dr_wav.h"
	)
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_LIBSNDFILE)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/libsndfile.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libsndfile.h")
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_LIBOPENMPT)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/libopenmpt.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libopenmpt.h")
endif()

if(CLOWNAUDIO_LIBXMP OR CLOWNAUDIO_LIBXMPLITE)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_LIBXMPLITE)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/libxmp-lite.c")
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libxmp-lite.h")
endif()

if(CLOWNAUDIO_PXTONE)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_PXTONE)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/pxtone.cpp"
		"src/decoding/decoders/pxtone.h"
		"src/decoding/decoders/pxtone_noise.cpp"
		"src/decoding/decoders/pxtone_noise.h"
		"src/decoding/decoders/libs/pxtone/pxtn.h"
		"src/decoding/decoders/libs/pxtone/pxtnDelay.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnDelay.h"
		"src/decoding/decoders/libs/pxtone/pxtnDescriptor.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnDescriptor.h"
		"src/decoding/decoders/libs/pxtone/pxtnError.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnError.h"
		"src/decoding/decoders/libs/pxtone/pxtnEvelist.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnEvelist.h"
		"src/decoding/decoders/libs/pxtone/pxtnMaster.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnMaster.h"
		"src/decoding/decoders/libs/pxtone/pxtnMax.h"
		"src/decoding/decoders/libs/pxtone/pxtnMem.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnMem.h"
		"src/decoding/decoders/libs/pxtone/pxtnOverDrive.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnOverDrive.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Frequency.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Frequency.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Noise.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Noise.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_NoiseBuilder.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oggv.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oggv.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oscillator.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_Oscillator.h"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_PCM.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnPulse_PCM.h"
		"src/decoding/decoders/libs/pxtone/pxtnService.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnService.h"
		"src/decoding/decoders/libs/pxtone/pxtnService_moo.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnText.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnText.h"
		"src/decoding/decoders/libs/pxtone/pxtnUnit.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnUnit.h"
		"src/decoding/decoders/libs/pxtone/pxtnWoice.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnWoice.h"
		"src/decoding/decoders/libs/pxtone/pxtnWoice_io.cpp"
		"src/decoding/decoders/libs/pxtone/pxtnWoicePTV.cpp"
		"src/decoding/decoders/libs/pxtone/pxtoneNoise.cpp"
		"src/decoding/decoders/libs/pxtone/pxtoneNoise.h"
	)
	list(APPEND STATIC_LIBS stdc++)
endif()

if(CLOWNAUDIO_SNES_SPC)
	target_compile_definitions(clownaudio PRIVATE CLOWNAUDIO_SNES_SPC)
	list(APPEND C_AND_CPP_SOURCES "src/decoding/decoders/snes_spc.c")
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/snes_spc.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_common.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_config.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_endian.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/blargg_source.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/dsp.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_misc.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SNES_SPC_state.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/spc.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/spc.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_CPU.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_DSP.h"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.cpp"
		"src/decoding/decoders/libs/snes_spc-0.9.0/snes_spc/SPC_Filter.h"
	)
endif()


#####################
# Playback backends #
#####################

if(NOT CLOWNAUDIO_MIXER_ONLY)
	if(CLOWNAUDIO_BACKEND STREQUAL "miniaudio")
		list(APPEND C_AND_CPP_SOURCES "src/playback/miniaudio.c")
		target_compile_definitions(clownaudio PRIVATE MINIAUDIO_ENABLE_DEVICE_IO)

		include(CheckLibraryExists)

		check_library_exists(m pow "" LIBM)
		if(LIBM)
			target_link_libraries(clownaudio PRIVATE m)
			list(APPEND STATIC_LIBS m)
		endif()

		check_library_exists(pthread pthread_create "" LIBPTHREAD)
		if(LIBPTHREAD)
			target_link_libraries(clownaudio PRIVATE pthread)
			list(APPEND STATIC_LIBS pthread)
		endif()

		target_link_libraries(clownaudio PRIVATE ${CMAKE_DL_LIBS})
		list(APPEND STATIC_LIBS ${CMAKE_DL_LIBS})
	elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL1")
		list(APPEND C_AND_CPP_SOURCES "src/playback/sdl1.c")
	elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
		list(APPEND C_AND_CPP_SOURCES "src/playback/sdl2.c")
	elseif(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
		list(APPEND C_AND_CPP_SOURCES "src/playback/cubeb.c")
	elseif(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
		list(APPEND C_AND_CPP_SOURCES "src/playback/portaudio.c")
	else()
		message(FATAL_ERROR "Invalid BACKEND selected")
	endif()
endif()


#####################
# Find dependencies #
#####################

# CMake

if(NOT CLOWNAUDIO_MIXER_ONLY)
	if(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
		find_package(cubeb REQUIRED)

		target_link_libraries(clownaudio PRIVATE cubeb::cubeb)
		list(APPEND STATIC_LIBS cubeb)
	endif()
endif()

# pkg-config

find_package(PkgConfig QUIET)

if(NOT CLOWNAUDIO_MIXER_ONLY)
	if(CLOWNAUDIO_BACKEND STREQUAL "SDL1")
		if(NOT PkgConfig_FOUND)
			message(FATAL_ERROR "pkg-config is needed by SDL1, but it couldn't be found")
		endif()

		pkg_check_modules(sdl REQUIRED IMPORTED_TARGET sdl)
		target_link_libraries(clownaudio PRIVATE PkgConfig::sdl)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} sdl")
	endif()

	if(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
		if(NOT PkgConfig_FOUND)
			message(FATAL_ERROR "pkg-config is needed by SDL2, but it couldn't be found")
		endif()

		pkg_check_modules(sdl2 REQUIRED IMPORTED_TARGET sdl2)
		target_link_libraries(clownaudio PRIVATE PkgConfig::sdl2)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} sdl2")
	endif()

	if(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
		if(NOT PkgConfig_FOUND)
			message(FATAL_ERROR "pkg-config is needed by PortAudio, but it couldn't be found")
		endif()

		pkg_check_modules(portaudio REQUIRED IMPORTED_TARGET portaudio-2.0)
		target_link_libraries(clownaudio PRIVATE PkgConfig::portaudio)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} portaudio-2.0")
	endif()
endif()

if(CLOWNAUDIO_LIBVORBIS)
	if(NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libvorbis, but it couldn't be found")
	endif()

	pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::vorbisfile)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} vorbisfile")
endif()

if(CLOWNAUDIO_LIBOPUS)
	if(NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libopus, but it couldn't be found")
	endif()

	pkg_check_modules(opusfile REQUIRED IMPORTED_TARGET opusfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::opusfile)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} opusfile")
endif()

if(CLOWNAUDIO_LIBFLAC)
	if(NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libflac, but it couldn't be found")
	endif()

	pkg_check_modules(flac REQUIRED IMPORTED_TARGET flac)
	target_link_libraries(clownaudio PRIVATE PkgConfig::flac)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} flac")
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	if(NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libsndfile, but it couldn't be found")
	endif()

	pkg_check_modules(sndfile REQUIRED IMPORTED_TARGET sndfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::sndfile)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} sndfile")
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	if(NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config is needed by libopenmpt, but it couldn't be found")
	endif()

	pkg_check_modules(libopenmpt REQUIRED IMPORTED_TARGET libopenmpt)
	target_link_libraries(clownaudio PRIVATE PkgConfig::libopenmpt)
	set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} libopenmpt")
endif()

if(CLOWNAUDIO_LIBXMP)
	if(PkgConfig_FOUND)
		pkg_check_modules(libxmp IMPORTED_TARGET libxmp)
	endif()

	if(libxmp_FOUND)
		message(STATUS "Using system libxmp")
		target_link_libraries(clownaudio PRIVATE PkgConfig::libxmp)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} libxmp")
	else()
		# Compile it ourselves
		message(STATUS "Using local libxmp")
		list(APPEND C_AND_CPP_SOURCES
			"src/decoding/decoders/libs/libxmp/src/control.c"
			"src/decoding/decoders/libs/libxmp/src/dataio.c"
			"src/decoding/decoders/libs/libxmp/src/effects.c"
			"src/decoding/decoders/libs/libxmp/src/extras.c"
			"src/decoding/decoders/libs/libxmp/src/filter.c"
			"src/decoding/decoders/libs/libxmp/src/format.c"
			"src/decoding/decoders/libs/libxmp/src/hio.c"
			"src/decoding/decoders/libs/libxmp/src/hmn_extras.c"
			"src/decoding/decoders/libs/libxmp/src/lfo.c"
			"src/decoding/decoders/libs/libxmp/src/load.c"
			"src/decoding/decoders/libs/libxmp/src/load_helpers.c"
			"src/decoding/decoders/libs/libxmp/src/md5.c"
			"src/decoding/decoders/libs/libxmp/src/med_extras.c"
			"src/decoding/decoders/libs/libxmp/src/memio.c"
			"src/decoding/decoders/libs/libxmp/src/misc.c"
			"src/decoding/decoders/libs/libxmp/src/mix_all.c"
			"src/decoding/decoders/libs/libxmp/src/mix_paula.c"
			"src/decoding/decoders/libs/libxmp/src/mixer.c"
			"src/decoding/decoders/libs/libxmp/src/mkstemp.c"
			"src/decoding/decoders/libs/libxmp/src/period.c"
			"src/decoding/decoders/libs/libxmp/src/player.c"
			"src/decoding/decoders/libs/libxmp/src/read_event.c"
			"src/decoding/decoders/libs/libxmp/src/scan.c"
			"src/decoding/decoders/libs/libxmp/src/smix.c"
			"src/decoding/decoders/libs/libxmp/src/tempfile.c"
			"src/decoding/decoders/libs/libxmp/src/virtual.c"
			"src/decoding/decoders/libs/libxmp/src/win32.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/arcfs.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/bunzip2.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/crc32.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/depacker.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/gunzip.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/inflate.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/mmcmp.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/muse.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/oxm.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/ppdepack.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/ptpopen.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/readlzw.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/readrle.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/s404_dec.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/unarc.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/uncompress.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/unlha.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/unlzx.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/unsqsh.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/unxz.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/unzip.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/vorbis.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/xfd.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/xfd_link.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/xfnmatch.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz_dec_lzma2.c"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz_dec_stream.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/669_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/abk_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/amf_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/arch_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/asif.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/asylum_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/chip_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/coco_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/common.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/dbm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/digi_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/dt_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/emod_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/far_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/flt_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/fnk_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/gal4_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/gal5_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/gdm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/hmn_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/ice_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/iff.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/imf_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/ims_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/it_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/itsex.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/liq_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/masi_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mdl_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/med2_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/med3_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/med4_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mfp_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mgt_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mmd_common.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mmd1_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mmd3_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mod_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/mtm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/no_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/okt_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/psm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/pt3_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/ptm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/pw_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/rtm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/s3m_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/sample.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/sfx_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/st_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/stim_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/stm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/stx_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/sym_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/ult_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/umx_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/voltable.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/xm_load.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/ac1d.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/di.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/eureka.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/fc-m.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/fuchs.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/fuzzac.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/gmc.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/heatseek.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/hrt.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/ksm.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/mp.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/noiserun.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/novotrade.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/np1.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/np2.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/np3.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/p40.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/p61a.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pha.c"
#			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pm.c"
#			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pm01.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pm10c.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pm18a.c"
#			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pm20.c"
#			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pm40.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pp10.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pp21.c"
#			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/pp30.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/prowiz.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/prun1.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/prun2.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/ptktable.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/skyt.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/starpack.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/tdd.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/theplayer.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/titanics.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/tp1.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/tp3.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/tuning.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/unic.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/unic2.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/wn.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/xann.c"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/zen.c"
		)
		if(WIN32)
			list(APPEND C_AND_CPP_SOURCES
				"src/decoding/decoders/libs/libxmp/src/win32.c"
			)
		endif()

		target_sources(clownaudio PRIVATE
			"src/decoding/decoders/libs/libxmp/include/xmp.h"
			"src/decoding/decoders/libs/libxmp/src/callbackio.h"
			"src/decoding/decoders/libs/libxmp/src/common.h"
			"src/decoding/decoders/libs/libxmp/src/effects.h"
			"src/decoding/decoders/libs/libxmp/src/extras.h"
			"src/decoding/decoders/libs/libxmp/src/fmopl.h"
			"src/decoding/decoders/libs/libxmp/src/format.h"
			"src/decoding/decoders/libs/libxmp/src/hio.h"
			"src/decoding/decoders/libs/libxmp/src/hmn_extras.h"
			"src/decoding/decoders/libs/libxmp/src/lfo.h"
			"src/decoding/decoders/libs/libxmp/src/list.h"
			"src/decoding/decoders/libs/libxmp/src/md5.h"
			"src/decoding/decoders/libs/libxmp/src/mdataio.h"
			"src/decoding/decoders/libs/libxmp/src/med_extras.h"
			"src/decoding/decoders/libs/libxmp/src/memio.h"
			"src/decoding/decoders/libs/libxmp/src/mixer.h"
			"src/decoding/decoders/libs/libxmp/src/paula.h"
			"src/decoding/decoders/libs/libxmp/src/period.h"
			"src/decoding/decoders/libs/libxmp/src/player.h"
			"src/decoding/decoders/libs/libxmp/src/precomp_blep.h"
			"src/decoding/decoders/libs/libxmp/src/precomp_lut.h"
			"src/decoding/decoders/libs/libxmp/src/tempfile.h"
			"src/decoding/decoders/libs/libxmp/src/virtual.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/crc32.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/depacker.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/inflate.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/ptpopen.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/readhuff.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/readlzw.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/readrle.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/vorbis.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/xfnmatch.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz_config.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz_lzma2.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz_private.h"
			"src/decoding/decoders/libs/libxmp/src/depackers/xz_stream.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/asif.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/iff.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/it.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/loader.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/med.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/mod.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/s3m.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/xm.h"
			"src/decoding/decoders/libs/libxmp/src/loaders/prowizard/prowiz.h"
		)

		target_include_directories(clownaudio PRIVATE "src/decoding/decoders/libs/libxmp/include")
		target_compile_definitions(clownaudio PRIVATE)
		if(MSVC)
			target_compile_definitions(clownaudio PRIVATE inline=_inline PATH_MAX=1024) # MSVC 6 doesn't have inline
		endif()
	endif()
endif()

if(CLOWNAUDIO_LIBXMPLITE)
	if(PkgConfig_FOUND)
		pkg_check_modules(libxmp-lite IMPORTED_TARGET libxmp-lite)
	endif()

	if(libxmp-lite_FOUND)
		message(STATUS "Using system libxmp-lite")
		target_link_libraries(clownaudio PRIVATE PkgConfig::libxmp-lite)
		set(PKG_CONFIG_REQUIRES "${PKG_CONFIG_REQUIRES} libxmp-lite")
	else()
		# Compile it ourselves
		message(STATUS "Using local libxmp-lite")
		list(APPEND C_AND_CPP_SOURCES
			"src/decoding/decoders/libs/libxmp-lite/src/control.c"
			"src/decoding/decoders/libs/libxmp-lite/src/dataio.c"
			"src/decoding/decoders/libs/libxmp-lite/src/effects.c"
			"src/decoding/decoders/libs/libxmp-lite/src/filter.c"
			"src/decoding/decoders/libs/libxmp-lite/src/format.c"
			"src/decoding/decoders/libs/libxmp-lite/src/hio.c"
			"src/decoding/decoders/libs/libxmp-lite/src/lfo.c"
			"src/decoding/decoders/libs/libxmp-lite/src/load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/load_helpers.c"
			"src/decoding/decoders/libs/libxmp-lite/src/md5.c"
			"src/decoding/decoders/libs/libxmp-lite/src/memio.c"
			"src/decoding/decoders/libs/libxmp-lite/src/misc.c"
			"src/decoding/decoders/libs/libxmp-lite/src/mix_all.c"
			"src/decoding/decoders/libs/libxmp-lite/src/mixer.c"
			"src/decoding/decoders/libs/libxmp-lite/src/period.c"
			"src/decoding/decoders/libs/libxmp-lite/src/player.c"
			"src/decoding/decoders/libs/libxmp-lite/src/read_event.c"
			"src/decoding/decoders/libs/libxmp-lite/src/scan.c"
			"src/decoding/decoders/libs/libxmp-lite/src/smix.c"
			"src/decoding/decoders/libs/libxmp-lite/src/virtual.c"
			"src/decoding/decoders/libs/libxmp-lite/src/win32.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/common.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/it_load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/itsex.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/mod_load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/s3m_load.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/sample.c"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/xm_load.c"
		)

		target_sources(clownaudio PRIVATE
			"src/decoding/decoders/libs/libxmp-lite/include/libxmp-lite/xmp.h"
			"src/decoding/decoders/libs/libxmp-lite/src/callbackio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/common.h"
			"src/decoding/decoders/libs/libxmp-lite/src/effects.h"
			"src/decoding/decoders/libs/libxmp-lite/src/format.h"
			"src/decoding/decoders/libs/libxmp-lite/src/hio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/lfo.h"
			"src/decoding/decoders/libs/libxmp-lite/src/list.h"
			"src/decoding/decoders/libs/libxmp-lite/src/md5.h"
			"src/decoding/decoders/libs/libxmp-lite/src/mdataio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/memio.h"
			"src/decoding/decoders/libs/libxmp-lite/src/mixer.h"
			"src/decoding/decoders/libs/libxmp-lite/src/period.h"
			"src/decoding/decoders/libs/libxmp-lite/src/player.h"
			"src/decoding/decoders/libs/libxmp-lite/src/precomp_lut.h"
			"src/decoding/decoders/libs/libxmp-lite/src/tempfile.h"
			"src/decoding/decoders/libs/libxmp-lite/src/virtual.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/it.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/loader.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/mod.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/s3m.h"
			"src/decoding/decoders/libs/libxmp-lite/src/loaders/xm.h"
		)

		target_include_directories(clownaudio PRIVATE "src/decoding/decoders/libs/libxmp-lite/include/libxmp-lite")
		target_compile_definitions(clownaudio PRIVATE LIBXMP_CORE_PLAYER=1 LIBXMP_NO_DEPACKERS=1)
		if(MSVC)
			target_compile_definitions(clownaudio PRIVATE inline=_inline) # MSVC 6 doesn't have inline
		endif()
	endif()
endif()


##########################################################
# Handle sources that can compile as either C99 or C++98 #
##########################################################

target_sources(clownaudio PRIVATE ${C_AND_CPP_SOURCES})
if(CLOWNAUDIO_CPP)
	set_source_files_properties(${C_AND_CPP_SOURCES} PROPERTIES LANGUAGE CXX)
endif()


##################
# Misc. settings #
##################

# Include the public headers
target_include_directories(clownaudio PUBLIC
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

# Export symbols
include(GenerateExportHeader)
GENERATE_EXPORT_HEADER(clownaudio EXPORT_FILE_NAME "${CMAKE_BINARY_DIR}/export/clownaudio_export.h")
target_include_directories(clownaudio PUBLIC "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/export>")

set_target_properties(clownaudio PROPERTIES C_VISIBILITY_PRESET hidden CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)

# Define public header
if(CLOWNAUDIO_MIXER_ONLY)
	set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "include/clownaudio/mixer.h;${CMAKE_BINARY_DIR}/export/clownaudio_export.h")
else()
	set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "include/clownaudio/clownaudio.h;include/clownaudio/playback.h;include/clownaudio/mixer.h;${CMAKE_BINARY_DIR}/export/clownaudio_export.h")
endif()

# Set language standards
# Extensions may be used by dependencies (miniaudio), so enable them
set_target_properties(clownaudio PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS ON
	CXX_STANDARD 11
	CXX_EXTENSIONS ON
)

# Shut up those stupid MSVC warnings
if(MSVC)
	target_compile_definitions(clownaudio PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Group files by directory when using an IDE
get_target_property(CLOWNAUDIO_SOURCES clownaudio SOURCES) 
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES CLOWNAUDIO_SOURCES)


###########
# Install #
###########

include(GNUInstallDirs)

# Install directories
install(TARGETS clownaudio
	EXPORT clownaudioTargets
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/clownaudio"
)

# Install `clownaudioConfig.cmake` and `clownaudioTargets.cmake`
install(EXPORT clownaudioTargets DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clownaudio")

export(TARGETS clownaudio FILE "clownaudioTargets.cmake")

include(CMakePackageConfigHelpers)
configure_package_config_file(
	"clownaudioConfig.cmake.in"
	"clownaudioConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clownaudio"
)

install(FILES "${CMAKE_BINARY_DIR}/clownaudioConfig.cmake" DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/clownaudio")

# pkg-config
if(DEFINED STATIC_LIBS)
	list(REMOVE_DUPLICATES STATIC_LIBS)
	foreach(ITEM ${STATIC_LIBS})
		set(PKG_CONFIG_STATIC_LIBS "${PKG_CONFIG_STATIC_LIBS} -l${ITEM}")
	endforeach()
endif()

configure_file("clownaudio.pc.in" "clownaudio.pc" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/clownaudio.pc" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig")
