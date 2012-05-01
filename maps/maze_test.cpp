/**
 * @file maze_test.cpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-05-01
 */

#include "maze.hpp"

int main( int argc, char *argv[] )
{
    using namespace maps;

    auto maze = Maze(31, 13, 1);

    for (size_t i = 0; i < maze.getWidth(); ++i) {
        for (size_t j = 0; j < maze.getHeight(); ++j) {
            std::cout << (maze.isPath(i, j) ? "  " : "##");
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
