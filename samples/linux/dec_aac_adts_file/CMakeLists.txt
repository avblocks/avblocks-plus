cmake_minimum_required(VERSION 3.16)

project(dec_aac_adts_file)
set (target dec_aac_adts_file)

add_executable(${target})

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin/${PLATFORM})

# debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC  _DEBUG)
endif()

# release definitions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG)
endif()

# Linux
if(OS STREQUAL "linux") 
    # common compile options
    target_compile_options(${target} PRIVATE -std=c++17 -MMD -MP -MF)

    # x64 compile options
    if (PLATFORM STREQUAL "x64") 
        target_compile_options(${target} PRIVATE -m64 -fPIC)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
    target_compile_options(${target} PRIVATE -m32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE -g)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE -O2 -s)
    endif()
endif()


# include dirs
target_include_directories(${target}
    PUBLIC
        ../../../include
        ../../../sdk/include
)

# sources
file(GLOB source "./*.cpp")


target_sources(${target}
PRIVATE
    ${source} 
)

# lib dirs
target_link_directories(${target}
PRIVATE
    # avblocks
    ${PROJECT_SOURCE_DIR}/../../../sdk/lib/${PLATFORM}
)

# libs
if(OS STREQUAL "linux")
    target_link_libraries(
        ${target}

        # primo-avblocks
        libAVBlocks64.so

        # os
        # pthread 
        # rt
    )
endif()
