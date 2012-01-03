cmake_minimum_required(VERSION 2.8)
project(sndgraph)
#set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++0x -ggdb3")

include_directories(.)

find_package(SDL REQUIRED)

add_executable(sndgraph sndgraph.cpp )
target_link_libraries(sndgraph ${SDL_LIBRARY})
