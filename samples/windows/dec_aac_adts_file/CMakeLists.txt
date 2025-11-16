cmake_minimum_required(VERSION 3.16)

project(dec_aac_adts_file)
set (target dec_aac_adts_file)

add_executable(${target})

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# output
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin/${PLATFORM})

if (CMAKE_GENERATOR MATCHES "Visual Studio.*20.*")
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../../bin)
endif()

# debug definitions
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${target} PUBLIC  _DEBUG)
endif()

# release definitions
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${target} PUBLIC NDEBUG)
endif()

# Windows
if(OS STREQUAL "windows")
    # add /MTd or /MT compiler option 
    set_property(TARGET ${target} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    # enable C++ 17 standard features
    target_compile_features(${target} PRIVATE cxx_std_17)

    # assume extern "C" functions do not throw. 
    # see: https://docs.microsoft.com/en-us/cpp/build/reference/eh-exception-handling-model?view=vs-2019
    target_compile_options(${target} PRIVATE /EHsc)

    # common definitions
    target_compile_definitions(${target} PRIVATE UNICODE _UNICODE)    
    
    # x64 compile options
    if (PLATFORM STREQUAL "x64") 
        target_compile_definitions(${target} PRIVATE WIN64)
    endif()

    # x86 compile options
    if (PLATFORM STREQUAL "x86") 
        target_compile_definitions(${target} PRIVATE WIN32)
    endif()

    # debug compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options(${target} PRIVATE /Zi /Od)
    endif()

    # release compile options
    if (CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(${target} PRIVATE /O2)
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
if(OS STREQUAL "windows")
    target_link_libraries(
        ${target}
        
        # primo-avblocks
        AVBlocks64.dll

        # os
        Shlwapi 
    )
endif()

