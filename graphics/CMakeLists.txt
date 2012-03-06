project(graphics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -ggdb3")

add_executable(gr
    client.cpp
    )
target_link_libraries(gr
    osg
    osgDB
    osgViewer
    osgGA
    OpenThreads
    )
