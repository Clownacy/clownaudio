cmake_minimum_required(VERSION 3.4)
project(libxmp-lite LANGUAGES C)

add_library(libxmp-lite OBJECT
	"include/libxmp-lite/xmp.h"
	"src/common.h"
	"src/control.c"
	"src/dataio.c"
	"src/effects.c"
	"src/effects.h"
	"src/filter.c"
	"src/format.c"
	"src/format.h"
	"src/hio.c"
	"src/hio.h"
	"src/lfo.c"
	"src/lfo.h"
	"src/list.h"
	"src/load.c"
	"src/load_helpers.c"
	"src/mdataio.h"
	"src/memio.c"
	"src/memio.h"
	"src/mix_all.c"
	"src/mixer.c"
	"src/mixer.h"
	"src/period.c"
	"src/period.h"
	"src/player.c"
	"src/player.h"
	"src/precomp_lut.h"
	"src/read_event.c"
	"src/scan.c"
	"src/smix.c"
	"src/tempfile.h"
	"src/virtual.c"
	"src/virtual.h"
	"src/loaders/common.c"
	"src/loaders/it.h"
	"src/loaders/it_load.c"
	"src/loaders/itsex.c"
	"src/loaders/loader.h"
	"src/loaders/mod.h"
	"src/loaders/mod_load.c"
	"src/loaders/s3m.h"
	"src/loaders/s3m_load.c"
	"src/loaders/sample.c"
	"src/loaders/xm.h"
	"src/loaders/xm_load.c"
)

# Do not export symbols
set_target_properties(libxmp-lite PROPERTIES C_VISIBILITY_PRESET hidden CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN ON)

target_compile_definitions(libxmp-lite PRIVATE LIBXMP_CORE_PLAYER=1)

target_include_directories(libxmp-lite PUBLIC "include/libxmp-lite")
target_include_directories(libxmp-lite PRIVATE "src")

if(MSVC)
	target_compile_definitions(libxmp-lite PRIVATE _CRT_SECURE_NO_WARNINGS=1 inline=__inline _USE_MATH_DEFINES=1)
endif()
