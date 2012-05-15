cmake_minimum_required(VERSION 2.8)
project(hexit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Weffc++ -pedantic -ggdb3")

add_library(maps
    maps/maze.cpp
    )

add_executable(maze_test
    maps/maze_test.cpp
    )
target_link_libraries( maze_test
    maps
    )

add_executable(client
    graphics/client.cpp
    )
target_link_libraries(client
    osg
    osgDB
    osgViewer
    osgGA
    OpenThreads
    maps
    )

include_directories(.)

find_package(SDL REQUIRED)

add_executable(sndgraph
    sound/sndgraph.cpp
    )
target_link_libraries(sndgraph
    ${SDL_LIBRARY}
    )

add_executable(engine_test
    engine/engine.cpp
    )
target_link_libraries(engine_test
    maps
    )

add_executable(network_client
    network/client.cpp)
add_executable(network_server
    network/server.cpp)
