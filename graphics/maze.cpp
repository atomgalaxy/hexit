
#include "mazegenerator.hpp"


int main( int argc, char *argv[] )
{
    int width = 81;
    int height = 81;
    auto mpair = generate_maze(width, height);
    for (size_t i = 0; i < width; ++i) {
        for (size_t j = 0; j < height; ++j) {
            std::cout << ((mpair.second[i][j]) ? "##" : "  ");
        }
        std::cout << std::endl;
    }

    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
