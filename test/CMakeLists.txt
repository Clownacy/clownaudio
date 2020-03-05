cmake_minimum_required(VERSION 3.14.6)

project(clownaudio_test LANGUAGES C)

add_executable(test "test.c")

add_subdirectory(".." "clownaudio" EXCLUDE_FROM_ALL)
target_link_libraries(test clownaudio)

set_target_properties(test PROPERTIES
	C_STANDARD 99
	C_EXTENSIONS OFF
)

if(MSVC)
	target_compile_definitions(test PRIVATE _CRT_SECURE_NO_WARNINGS)	# Shut up those stupid warnings
endif()
