# NOTE: keep it here to not forget
# cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
# if(CMAKE_BUILD_TYPE STREQUAL "Release")

cmake_minimum_required(VERSION 3.21)
project(rsoundboard)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(raylib_EXAMPLES OFF CACHE BOOL "Build raylib examples")
set(raylib_TESTS OFF CACHE BOOL "Build raylib tests")


option(IS_DEBUG_BUILD "Is debug build" ON)

set(PROJECT_SRC
	src/main.c
	src/keyboard_hook.c

	src/external/tinyfiledialogs/tinyfiledialogs.c
	src/external/raygui/raygui.c
	)

add_executable(${PROJECT_NAME} ${PROJECT_SRC})

if (IS_DEBUG_BUILD)
	target_compile_definitions(${PROJECT_NAME} PUBLIC "DEBUG_BUILD=1")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE src)
target_include_directories(${PROJECT_NAME} PRIVATE src/external/raylib/src)
target_include_directories(${PROJECT_NAME} PRIVATE src/external/raygui/)
target_include_directories(${PROJECT_NAME} PRIVATE src/external/tinyfiledialogs/)

if(WIN32)
	if(IS_DEBUG_BUILD)
        # set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")

        add_subdirectory(src/external/raylib)
		target_link_libraries(${PROJECT_NAME} raylib)

		if (MSVC)
			target_compile_options(${PROJECT_NAME} PRIVATE /W4)
		else ()
			target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
		endif ()
	endif()
else()
	message(FATAL_ERROR "Unsupported or unknown platform")
endif()