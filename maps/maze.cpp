
#include "../misc/utility.hpp"
#include "maze.hpp"

#include <type_traits>


namespace maps {
/** Initializes the borders and makes all other ground passable */
void Maze::initialize_maze() {
    for (size_t i = 1; i < width-1; ++i) {
        for (size_t j = 1; j < height-1; ++j) {
            setPath(i, j, PathTypes::NORMAL);
        }
    }
    /* fill borders */
    for (size_t i = 0; i < width; ++i) {
        setWall(i, 0, WallTypes::BORDER);
    }
    for (size_t j = 0; j < height; ++j) {
        setWall(0, j, WallTypes::BORDER);
    }
}

/** makes the walls of the maze. */
void Maze::make_walls() {
    using std::make_pair;
    using utility::rand;

    size_t complexity = size_t(this->complexity*(5*(width + height)));
    size_t density    = size_t(this->density*width/2*height/2);

    for (size_t i = 0; i < density; ++i) {
        size_t x = rand(0, width/2)*2;
        size_t y = rand(0, height/2)*2;
        setWall(x, y, WallTypes::INNER);

        std::vector<std::pair<size_t, size_t>> neigh;
        neigh.reserve(4);
        for (size_t j = 0; j < complexity; ++j) {
            // add all neighbors that are not out of bounds
            if (x > 1)        { neigh.push_back( make_pair(x-2, y) ); }
            if (x < height-2) { neigh.push_back( make_pair(x+2, y) ); }
            if (y > 1)        { neigh.push_back( make_pair(x, y-2) ); }
            if (y < width-2)  { neigh.push_back( make_pair(x, y+2) ); }
            if (neigh.size()) { // choose a random neighbor if there are any
                auto n = neigh[rand(0, neigh.size())];
                if (isPath(n.first, n.second)) {
                    auto link = make_pair(
                            x + static_cast<long long>(n.first - x)/2,
                            y + static_cast<long long>(n.second - y)/2);
                    setWall(n.first,    n.second,    WallTypes::INNER);
                    setWall(link.first, link.second, WallTypes::INNER);
                    x = n.first;
                    y = n.second;
                }
            }
            neigh.clear();
        }
    }

}

/**
 * Finds all blind ends in the maze.
 *
 * The return value is a pair of (corner, approach)
 * where the corner is the corner square of the path
 * and the approach is the only adjacent path square direction.
 * 
 * Example: (a is the approach, c is the corner, # is wall)
 * <pre>
 * ######
 *    ac#
 * ######
 * </pre>
 * For the above maze, the function returns
 * ((4,1)(-1,0))
 */
std::vector<std::pair<std::pair<size_t, size_t>, std::pair<int, int>>>
Maze::find_blind_ends() {
    using std::make_pair;
    using std::pair;

    std::vector<pair<pair<size_t, size_t>, pair<int, int>>> koti;
    std::vector<pair<size_t, size_t>> neighbors
        {
            make_pair(-1,1),  make_pair(0,1),  make_pair(1,1),
            make_pair(-1,0),                   make_pair(1,0),
            make_pair(-1,-1), make_pair(0,-1), make_pair(1,-1)
        };

    for (size_t row = 1; row < height-1; ++row) {
        for (size_t col = 1; col < width-1; ++col) {
            if (isWall(col, row)) { continue; }

            auto monster = make_pair(0,0);
            size_t kot = 0;
            for (auto n : neighbors) {
                auto x = col + n.first;
                auto y = row + n.second;
                if (isWall(x, y)) {
                    kot += 1;
                } else {
                    monster = n;
                }
            }
            if (kot == 7) {
                koti.push_back(make_pair(make_pair(col, row),monster));
            }
        }
    }
    return koti;
}

void Maze::place_treasure_with_guardian_monsters() {
    using std::make_pair;
    using utility::random_shuffle;
    auto koti = find_blind_ends();
    random_shuffle(koti);

    size_t how_many_monsters = 10;
    treasure.reserve(how_many_monsters);
    monsters.reserve(how_many_monsters);

    for (size_t i = 0; i < std::min(koti.size(), how_many_monsters); ++i) {
        auto treasure = koti[i].first;
        auto monster_offset = koti[i].second;
        auto monster = make_pair(
                treasure.first + monster_offset.first,
                treasure.second + monster_offset.second);
        this->treasure.push_back(
                Object{
                    make_pair(treasure.first, treasure.second),
                    ObjectType::TREASURE, // type
                    5 // value
                });
        this->monsters.push_back(
                Object{
                    make_pair(monster.first, monster.second),
                    ObjectType::MONSTER, // type
                    0
                });
    }
}

void Maze::place_wondering_monsters()
{
    using std::make_pair;
    using utility::rand;
    /* wondering monsters */
    for (size_t i = 1; i < 40; i++) {
        size_t w = rand(1, width-1);
        size_t h = rand(1, height-1);
        if (isPath(w, h)) {
            monsters.push_back(
                    Object{
                        make_pair(w, h),
                        ObjectType::MONSTER,
                        1
                    });
        }
    }
}

bool Maze::is_in_center_third(size_t x, size_t y) {
    /*random start */
    auto row_third = (width-1)/3;
    auto col_third = (height-1)/3;
    return (row_third < x && x < 2*row_third) &&
           (col_third < y && y < 2*col_third);

}
std::pair<size_t, size_t> Maze::quadrant(size_t x, size_t y) {
    using std::make_pair;
    auto row_half = (width-1)/2;
    auto col_half = (height-1)/2;
    return make_pair(x/row_half, y/col_half);
}
void Maze::place_start()
{
    using std::make_pair;
    using utility::rand;
    do { // try to get a good start position
        start = make_pair(rand(1,width-1), rand(1, height-1));
        // and repeat if we failed and the found coordinates are in the
        // center third
    } while (!(
                !is_in_center_third(start.first, start.second) &&
                isPath(start.first, start.second)));
}

void Maze::place_end() {
    // make sure start has been set
    assert(start.first != 0 && start.second != 0);

    using std::make_pair;
    using utility::rand;

    auto start_quadrant = quadrant(start.first, start.second);

    do {
        finish = make_pair(rand(1, width-1), rand(1, height-1));
    } while (!(
                !is_in_center_third(finish.first, finish.second) &&
                start_quadrant != quadrant(finish.first, finish.second)
              ));
}

} //end namespace maps
