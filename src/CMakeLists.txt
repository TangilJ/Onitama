cmake_minimum_required(VERSION 3.17)
project(OnitamaCore)

set(CMAKE_CXX_STANDARD 17)

add_library(
        OnitamaCore

        core/data.h
        core/movegen.cpp
        core/movegen.h
        core/utilities.cpp
        core/utilities.h
        core/search.cpp
        core/search.h
)
target_include_directories(OnitamaCore INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)