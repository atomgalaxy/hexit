#ifndef MAZE_HPP_GUARD
#define MAZE_HPP_GUARD
/**
 * @file maze.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-05-01
 */

#include <boost/multi_array.hpp>
#include <utility>

namespace maps {

enum class WallTypes : unsigned int {
    BORDER,
    INNER
};

enum class PathTypes : unsigned int {
    NORMAL,
    GRASSY
};

enum class ObjectType {
    TREASURE,
    MONSTER,
    START,
    END
};

struct Object {
    std::pair<size_t, size_t> position;
    ObjectType type;
    // difficulty for monsters (higer - more difficult)
    // type for treasures (maybe upgrade goo?)
    unsigned int value;
};

class Maze {
    private:
    enum class FieldTypes : unsigned int {
        TYPE_MASK = 0xff000000,
        PATH = 0x01000000,
        WALL = 0x02000000,
    };

    typedef boost::multi_array<FieldTypes, 2> maze_type;

    size_t width;
    size_t height;

    double difficulty;

    double density;
    double complexity;

    decltype(boost::extents[width][height]) shape;
    maze_type maze;

    std::vector<Object> monsters;
    std::vector<Object> treasure;
    std::pair<size_t, size_t> start;
    std::pair<size_t, size_t> finish;

    std::vector<std::pair<std::pair<size_t, size_t>, std::pair<int, int>>>
    find_blind_ends();
    bool is_in_center_third(size_t x, size_t y);
    std::pair<size_t, size_t> quadrant(size_t x, size_t y);

    void initialize_maze();
    void make_walls();
    void place_treasure_with_guardian_monsters();
    void place_wondering_monsters();
    void place_start();
    void place_end();

    inline void
    generate_maze()
    {
        initialize_maze();
        make_walls();
        place_treasure_with_guardian_monsters();
        place_wondering_monsters();
        place_start();
        place_end();
    }

    inline void
    setPath(size_t x, size_t y, PathTypes t) {
//        std::cerr << "x: " << x << " y: " << y << std::endl;
        assert(x < width);
        assert(y < height);
        using ult = std::underlying_type<FieldTypes>::type;
        maze[x][y] = static_cast<FieldTypes>(
                static_cast<ult>(t) |
                static_cast<ult>(FieldTypes::PATH)
                );
    }

    inline void
    setWall(size_t x, size_t y, WallTypes t) {
//        std::cerr << "x: " << x << " y: " << y << std::endl;
        assert(x < width);
        assert(y < height);
        using ult = std::underlying_type<FieldTypes>::type;
        maze[x][y] = static_cast<FieldTypes>(
                static_cast<ult>(t) |
                static_cast<ult>(FieldTypes::WALL)
                );
    }

    inline FieldTypes
    getType(size_t x, size_t y) const {
        assert(x < width);
        assert(y < height);
        return
            static_cast<FieldTypes>(
                static_cast<unsigned int>(maze[x][y]) &
                static_cast<unsigned int>(FieldTypes::TYPE_MASK));
    }

    public:

    Maze(size_t width, size_t height, double difficulty)
        : width( (width/2)  * 2 + 1)
        , height((height/2) * 2 + 1)
        , difficulty(difficulty)
        , density(0.75)
        , complexity(0.75)
        , shape(boost::extents[width][height])
        , maze(shape)
        , monsters()
        , treasure()
        , start(0,0)
        , finish(0,0)
    {
        generate_maze();
    }

    inline bool
    isWall(size_t x, size_t y) const {
        assert(x < width);
        assert(y < height);
        return getType(x, y) == FieldTypes::WALL;
    }

    inline bool
    isPath(size_t x, size_t y) const {
        assert(x < width);
        assert(y < height);
        return getType(x, y) == FieldTypes::PATH;
    }

    inline WallTypes
    getWallType(size_t x, size_t y) const {
        assert(x < width);
        assert(y < height);
        assert(getType(x, y) == FieldTypes::WALL);
        using ult = std::underlying_type<FieldTypes>::type;
        return static_cast<WallTypes>(
                    static_cast<ult>(maze[x][y]) &
                    ~static_cast<ult>(FieldTypes::TYPE_MASK)
                );
    }

    inline PathTypes
    getPathType(size_t x, size_t y) const {
        assert(x < width);
        assert(y < height);
        assert(getType(x, y) == FieldTypes::WALL);
        using ult = std::underlying_type<FieldTypes>::type;
        return static_cast<PathTypes>(
                    static_cast<ult>(maze[x][y]) &
                    ~static_cast<ult>(FieldTypes::TYPE_MASK)
                );
    }

    decltype(width)  getWidth()  const { return width; }
    decltype(height) getHeight() const { return height; }

    decltype(start) getStart() const { return start; }
    decltype(finish) getFinish() const { return finish; }
};
} // end namespace maps

#endif
