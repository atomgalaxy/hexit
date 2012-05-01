project(maps)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -Weffc++ -pedantic -ggdb3")

add_executable(maps maze_test.cpp "maze.cpp")
