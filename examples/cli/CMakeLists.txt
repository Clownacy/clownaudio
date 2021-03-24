cmake_minimum_required(VERSION 3.12)

option(COMPILE_AS_CPP "Compile the program as C++ instead of C, for compilers that don't support C99 but do support C++98" OFF)
option(SUBDIRECTORY "Use clownaudio as a subdirectory, instead of an external package" OFF)

if(COMPILE_AS_CPP)
	set(LANGUAGES CXX)
else()
	set(LANGUAGES C)
endif()

project(clownaudio_test LANGUAGES ${LANGUAGES})

add_executable(test "test.c")

if(COMPILE_AS_CPP)
	set_source_files_properties("test.c" PROPERTIES LANGUAGE CXX)
endif()

if(SUBDIRECTORY)
	add_subdirectory("../.." "clownaudio" EXCLUDE_FROM_ALL)
else()
	find_package(clownaudio REQUIRED)
endif()

target_link_libraries(test PRIVATE clownaudio)

set_target_properties(test PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS OFF
	CXX_STANDARD 98
	CXX_EXTENSIONS OFF
)

if(MSVC)
	target_compile_definitions(test PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()
