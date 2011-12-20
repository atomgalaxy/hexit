/**
 * @file sample.cpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2011-12-20
 */

#include <AL/alure.h>
#include <vector>
#include <utility>
#include <iostream>

template <typename T>
struct play_message
{
    play_message(const buffer<T>& buf, double volume, double start_time, double
            stop_time)
        : buf(buf)
        , volume(volume)
        , start_time(start_time)
        , end_time(end_time)
    {}

    const buffer<T>& buf;
    double volume;
    double start_time;
    double stop_time;
};

template <typename T>
struct buffer
{
    typedef typename T value_type;
    std::vector<T> buf;
    size_t sampling_freq;
};

class player
{
public:
    player()
        : ok(true)
    {
        if (!alureInitDevice(nullptr, nullptr)) {
            ok = false;
            std::cerr << "Failed to open audio device. " <<
                alureGetErrorString() << std::endl;
        }
    }

    player(player& o) = delete;

    ~player()
    {
        alureShutdownDevice();
    }

    std::vector<std::string>
    get_device_names()
    {
        std::vector<std::string> names;
        ALCsizei count = 0;
        const ALCchar** dnames = alureGetDeviceNames(true, &count);
        for (size_t i = 0; i < count; ++i) {
            names.push_back(std::string(dnames[i]));
        }
        alureFreeDeviceNames(dnames);
        return names;
    }

    double
    get_time() {

    }
private:
    bool ok;
};

int main( int argc, char *argv[] )
{
    return 0;
}               /* ----------  end of function main  ---------- */
