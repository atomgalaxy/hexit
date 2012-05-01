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
