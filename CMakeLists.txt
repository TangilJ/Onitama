cmake_minimum_required(VERSION 3.17)
project(Onitama)

set(CMAKE_CXX_STANDARD 17)
add_definitions("-march=native -Ofast")
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
)