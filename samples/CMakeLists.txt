cmake_minimum_required(VERSION 3.16)

# meta project that includes all other projects
project(samples)

# Operating System
string(TOLOWER ${CMAKE_SYSTEM_NAME} OS)

# add projects, each project should be in its own directory
if (OS STREQUAL "darwin")
    add_subdirectory(${OS}/dec_aac_adts_file)
endif()

if(OS STREQUAL "linux")
    add_subdirectory(${OS}/dec_aac_adts_file)
endif()

if(OS STREQUAL "windows")
    add_subdirectory(${OS}/dec_aac_adts_file)
endif()
