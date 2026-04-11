cmake_minimum_required(VERSION 3.16)

project(info_metadata_file)
set (target info_metadata_file)

add_executable(${target})

string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin/${PLATFORM})

if (CMAKE_GENERATOR STREQUAL "Xcode")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin)
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC _DEBUG)
endif()
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG)
endif()

if(OS STREQUAL "darwin")
    target_compile_options(${target} PRIVATE -std=c++20 -stdlib=libc++)
    if (PLATFORM STREQUAL "x64")
        target_compile_options(${target} PRIVATE -m64 -fPIC)
    endif()
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE -g)
    endif()
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE -Os)
    endif()
endif()

target_include_directories(${target} PUBLIC
    ../../../include
    ../../../sdk/include
)

file(GLOB source "./*.cpp" "./*.mm")
target_sources(${target} PRIVATE ${source})

target_link_directories(${target} PRIVATE
    ../../../sdk/lib/${PLATFORM}
)

if (OS STREQUAL "darwin")
    target_link_libraries(${target}
        libAVBlocks.dylib
        "-framework CoreFoundation"
        "-framework AppKit"
    )
endif()
