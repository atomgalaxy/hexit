project(aluresample)
cmake_minimum_required(VERSION 2.8)

add_definitions("-pedantic -Wall -Weffc++ -std=gnu++0x -ggdb3")

add_executable( sample
    sample.cpp
    )

target_link_libraries( sample
    alure
    openal
    )
