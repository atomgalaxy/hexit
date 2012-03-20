#ifndef MAZEGENERATOR_HPP
#define MAZEGENERATOR_HPP
/**
 * @file mazegenerator.hpp
 * 
 *
 * @author Gal Kranjc
 * @since 2012-03-20
 */

#include <boost/multi_array.hpp>
#include <cstdlib>
#include <utility>
#include <cassert>

typedef boost::multi_array<int, 2> maze_type;

auto
generate_maze(
    int width, int height,
    double complexity = 0.75, double density = 0.75
    ) -> decltype(make_pair(boost::extents[width][height], maze_type()))
{
    using std::make_pair;
    using std::tie;

    assert(width % 2 == 1);
    assert(height % 2 == 1);

    complexity = int(complexity*(5*(width+height)));

    density = int(density*(width-1)*(height-1)/4);

    auto shape = boost::extents[width][height];
    maze_type maze(shape);

    for (int i = 1; i < width; ++i) {
        for (int j = 1; j < height; ++j) {
            maze[i][j] = 0;
        }
    }

    /* fill borders */
    for (int i = 0; i < width; ++i) {
        maze[i][0] = 1;
        maze[i][height-1] = 1;
    }

    for (int i = 0; i < height; ++i) {
        maze[0][i] = 1;
        maze[width-1][i] = 1;
    }

    for (int i = 0; i < density; ++i) {
        int x = (rand()%(width/2))*2;
        int y = (rand()%(height/2))*2;
        maze[x][y] = 1;

        for (int j = 0; j < complexity; ++j) {
            std::vector<std::pair<int, int>> neigh;
            if (x > 1) {
                neigh.push_back( make_pair(x-2, y) );
            }
            if (x < width-2) {
                neigh.push_back( make_pair(x+2, y) );
            }
            if (y > 1) {
                neigh.push_back( make_pair(x, y-2) );
            }
            if (y < height-2) {
                neigh.push_back( make_pair(x, y+2) );
            }
            if (neigh.size()) {
                auto lnm1 = neigh.size();
                int x_, y_;
                std::tie(x_, y_) = neigh[rand()%lnm1];
                if (maze[x_][y_] == 0) {
                    maze[x_][y_] = 1;
                    maze[x_ + (x - x_)/2][y_ + (y - y_)/2] = 1;
                    x = x_;
                    y = y_;
                }
            }
        }
    }
    return make_pair(shape, maze);
}



#endif
