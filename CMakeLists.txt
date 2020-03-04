cmake_minimum_required(VERSION 3.14.6)

option(CLOWNAUDIO_LIBVORBIS "Enable the libvorbis decoder backend" OFF)
option(CLOWNAUDIO_STB_VORBIS "Enable the stb_vorbis decoder backend" ON)
option(CLOWNAUDIO_LIBFLAC "Enable the libFLAC decoder backend" OFF)
option(CLOWNAUDIO_DR_FLAC "Enable the dr_flac decoder backend" ON)
option(CLOWNAUDIO_DR_WAV "Enable the dr_wav decoder backend" ON)
option(CLOWNAUDIO_LIBOPUS "Enable the libopus decoder backend" OFF)
option(CLOWNAUDIO_LIBSNDFILE "Enable the libsndfile decoder backend" OFF)
option(CLOWNAUDIO_LIBOPENMPT "Enable the libopenmpt decoder backend" OFF)
option(CLOWNAUDIO_LIBXMPLITE "Enable the libxmp-lite decoder backend" OFF)
option(CLOWNAUDIO_SNES_SPC "Enable the snes_spc decoder backend" OFF)
option(CLOWNAUDIO_PXTONE "Enable the PxTone decoder backend" OFF)
option(CLOWNAUDIO_NO_PLAYBACK "Disables playback capabilities" OFF)
set(CLOWNAUDIO_BACKEND "miniaudio" CACHE STRING "Which playback backend to use: supported options are 'miniaudio', 'SDL1', 'SDL2', 'Cubeb', and 'PortAudio'")

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_CURRENT_LIST_DIR}/cmake"
)

project(clownaudio LANGUAGES C CXX)

set(SOURCES
	"include/clownaudio/mixer.h"
	"src/miniaudio.c"
	"src/miniaudio.h"
	"src/mixer.c"
	"src/decoding/decoder_selector.c"
	"src/decoding/decoder_selector.h"
	"src/decoding/predecoder.c"
	"src/decoding/predecoder.h"
	"src/decoding/resampled_decoder.c"
	"src/decoding/resampled_decoder.h"
	"src/decoding/split_decoder.c"
	"src/decoding/split_decoder.h"
	"src/decoding/decoders/common.h"
	"src/decoding/decoders/memory_stream.c"
	"src/decoding/decoders/memory_stream.h"
)

if(NOT CLOWNAUDIO_NO_PLAYBACK)
	list(APPEND SOURCES
		"include/clownaudio/clownaudio.h"
		"src/clownaudio.c"
		"src/playback/playback.h"
	)
endif()

if(BUILD_SHARED_LIBS)
	add_library(clownaudio SHARED ${SOURCES})
else()
	add_library(clownaudio STATIC ${SOURCES})
endif()

####################
# Misc. settings
####################

target_include_directories(clownaudio INTERFACE "include")
target_include_directories(clownaudio PRIVATE "include/clownaudio")

if(BUILD_SHARED_LIBS AND WIN32)
	target_compile_definitions(clownaudio PRIVATE BUILDING_DLL)
endif()

if(CLOWNAUDIO_NO_PLAYBACK)
	set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "include/clownaudio/mixer.h")
else()
	set_target_properties(clownaudio PROPERTIES PUBLIC_HEADER "include/clownaudio/clownaudio.h;include/clownaudio/mixer.h")
endif()

set_target_properties(clownaudio PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS OFF
	CXX_STANDARD 11
	CXX_EXTENSIONS OFF
)

if(MSVC)
	target_compile_definitions(clownaudio PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid MSVC warnings
endif()

####################
# Decoding backends
####################

if(CLOWNAUDIO_LIBVORBIS)
	target_compile_definitions(clownaudio PRIVATE USE_LIBVORBIS)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libvorbis.c" "src/decoding/decoders/libvorbis.h")
endif()

if(CLOWNAUDIO_STB_VORBIS)
	target_compile_definitions(clownaudio PRIVATE USE_STB_VORBIS)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/stb_vorbis.c" "src/decoding/decoders/stb_vorbis.h")
endif()

if(CLOWNAUDIO_LIBFLAC)
	target_compile_definitions(clownaudio PRIVATE USE_LIBFLAC)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libflac.c" "src/decoding/decoders/libflac.h")
endif()

if(CLOWNAUDIO_DR_FLAC)
	target_compile_definitions(clownaudio PRIVATE USE_DR_FLAC)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/dr_flac.c" "src/decoding/decoders/dr_flac.h" "src/decoding/decoders/libs/dr_flac.h")
endif()

if(CLOWNAUDIO_DR_WAV)
	target_compile_definitions(clownaudio PRIVATE USE_DR_WAV)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/dr_wav.c" "src/decoding/decoders/dr_wav.h" "src/decoding/decoders/libs/dr_wav.h")
endif()

if(CLOWNAUDIO_LIBOPUS)
	target_compile_definitions(clownaudio PRIVATE USE_LIBOPUS)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libopus.c" "src/decoding/decoders/libopus.h")
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	target_compile_definitions(clownaudio PRIVATE USE_LIBSNDFILE)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libsndfile.c" "src/decoding/decoders/libsndfile.h")
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	target_compile_definitions(clownaudio PRIVATE USE_LIBOPENMPT)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libopenmpt.c" "src/decoding/decoders/libopenmpt.h")
endif()

if(CLOWNAUDIO_LIBXMPLITE)
	target_compile_definitions(clownaudio PRIVATE USE_LIBXMPLITE)
	target_sources(clownaudio PRIVATE "src/decoding/decoders/libxmp-lite.c" "src/decoding/decoders/libxmp-lite.h")
endif()

if(CLOWNAUDIO_SNES_SPC)
	target_compile_definitions(clownaudio PRIVATE USE_SNES_SPC)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/snes_spc.c" "src/decoding/decoders/snes_spc.h"
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

if(CLOWNAUDIO_PXTONE)
	target_compile_definitions(clownaudio PRIVATE USE_PXTONE)
	target_sources(clownaudio PRIVATE
		"src/decoding/decoders/pxtone.cpp" "src/decoding/decoders/pxtone.h"
		"src/decoding/decoders/pxtone_noise.cpp" "src/decoding/decoders/pxtone_noise.h"
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

####################
# Playback backends
####################

if(NOT CLOWNAUDIO_NO_PLAYBACK)

if(CLOWNAUDIO_BACKEND STREQUAL "miniaudio")
	target_sources(clownaudio PRIVATE "src/playback/miniaudio.c")
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
	target_sources(clownaudio PRIVATE "src/playback/sdl1.c")
elseif(CLOWNAUDIO_BACKEND STREQUAL "SDL2")
	target_sources(clownaudio PRIVATE "src/playback/sdl2.c")
elseif(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
	target_sources(clownaudio PRIVATE "src/playback/cubeb.c")
elseif(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
	target_sources(clownaudio PRIVATE "src/playback/portaudio.c")
else()
	message(FATAL_ERROR "Invalid BACKEND selected")
endif()

endif()

####################
# Find dependencies
####################

# CMake

if(NOT CLOWNAUDIO_NO_PLAYBACK)

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

if(CLOWNAUDIO_BACKEND STREQUAL "Cubeb")
	find_package(cubeb REQUIRED)

	target_link_libraries(clownaudio PRIVATE cubeb::cubeb)
	list(APPEND STATIC_LIBS cubeb)
endif()

endif()

if(CLOWNAUDIO_LIBXMPLITE)
	find_package(LibXMPLite)	# TODO - replace this with pkg-config

	if(libxmp-lite_FOUND)
		message(STATUS "Using system libxmp-lite")
		target_link_libraries(clownaudio PRIVATE libxmp-lite)
		list(APPEND STATIC_LIBS ${LIBXMPLITE_LIBRARIES})
	else()
		# Compile it ourselves
		message(STATUS "Using local libxmp-lite")
		add_subdirectory("src/decoding/decoders/libs/libxmp-lite" EXCLUDE_FROM_ALL)
		target_link_libraries(clownaudio PRIVATE libxmp-lite)
	endif()
endif()

# pkg-config

find_package(PkgConfig QUIET)

if(NOT CLOWNAUDIO_NO_PLAYBACK)

if(CLOWNAUDIO_BACKEND STREQUAL "PortAudio")
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config needed, but couldn't be found")
	endif()

	pkg_check_modules(portaudio REQUIRED IMPORTED_TARGET portaudio-2.0)
	target_link_libraries(clownaudio PRIVATE PkgConfig::portaudio)
	list(APPEND STATIC_LIBS ${portaudio_LIBRARIES})
endif()

endif()

if(CLOWNAUDIO_LIBVORBIS)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config needed, but couldn't be found")
	endif()

	pkg_check_modules(vorbisfile REQUIRED IMPORTED_TARGET vorbisfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::vorbisfile)
	list(APPEND STATIC_LIBS ${vorbisfile_STATIC_LIBRARIES})
endif()

if(CLOWNAUDIO_LIBFLAC)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config needed, but couldn't be found")
	endif()

	pkg_check_modules(flac REQUIRED IMPORTED_TARGET flac)
	target_link_libraries(clownaudio PRIVATE PkgConfig::flac)
	list(APPEND STATIC_LIBS ${flac_STATIC_LIBRARIES})
endif()

if(CLOWNAUDIO_LIBOPUS)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config needed, but couldn't be found")
	endif()

	pkg_check_modules(opusfile REQUIRED IMPORTED_TARGET opusfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::opusfile)
	list(APPEND STATIC_LIBS ${opusfile_STATIC_LIBRARIES})
endif()

if(CLOWNAUDIO_LIBSNDFILE)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config needed, but couldn't be found")
	endif()

	pkg_check_modules(sndfile REQUIRED IMPORTED_TARGET sndfile)
	target_link_libraries(clownaudio PRIVATE PkgConfig::sndfile)
	list(APPEND STATIC_LIBS ${sndfile_STATIC_LIBRARIES})
endif()

if(CLOWNAUDIO_LIBOPENMPT)
	if (NOT PkgConfig_FOUND)
		message(FATAL_ERROR "pkg-config needed, but couldn't be found")
	endif()

	pkg_check_modules(libopenmpt REQUIRED IMPORTED_TARGET libopenmpt)
	target_link_libraries(clownaudio PRIVATE PkgConfig::libopenmpt)
	list(APPEND STATIC_LIBS ${libopenmpt_STATIC_LIBRARIES})
endif()

####################
# Install
####################

include(GNUInstallDirs)

# Install directories
install(TARGETS clownaudio
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
	PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/clownaudio"
)

# pkg-config
list(REMOVE_DUPLICATES STATIC_LIBS)
foreach(ITEM ${STATIC_LIBS})
	set(PKG_CONFIG_STATIC_LIBS "-l${ITEM} ${PKG_CONFIG_STATIC_LIBS}")
endforeach()

configure_file("clownaudio.pc.in" clownaudio.pc @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/clownaudio.pc" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig")
