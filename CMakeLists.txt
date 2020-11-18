cmake_minimum_required(VERSION 3.17)
project(Onitama)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions("-march=native -O0")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_definitions("-march=native -Ofast")
else ()
    message(FATAL_ERROR "CMAKE_BUILD_TYPE was not 'Debug' or 'Release'")
endif ()

if (WIN32)
    link_libraries(ws2_32)
endif ()

add_executable(
        Onitama
        src/main.cpp
        src/main.h
        src/data.h
        src/perft.cpp
        src/perft.h
        src/movegen.cpp
        src/movegen.h
        src/utilities.cpp
        src/utilities.h
        src/search.cpp
        src/search.h
        extern/easywsclient/easywsclient.cpp
        extern/easywsclient/easywsclient.hpp
        src/cli.cpp
        src/cli.h
        src/zobrist.cpp
        src/zobrist.h
)

add_subdirectory(extern/CLI11)
target_link_libraries(Onitama CLI11::CLI11)

add_subdirectory(extern/json)
target_link_libraries(Onitama nlohmann_json::nlohmann_json)

target_include_directories(Onitama PRIVATE extern/easywsclient)