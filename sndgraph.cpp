/**
 * @file sndgraph.cc
 * Main file for the sngraph app.
 *
 * @author Gašper Ažman (GA), gasper.azman@gmail.com
 * @version 1.0
 * @since 2010-06-03 03:50:57 PM
 */

#include <cstdlib>
#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>
#include <cmath>
#include <iostream>
#include <vector>
#include <list>
#include <unistd.h>
#include <cassert>

#include <queue>
#include <sstream>

struct pravokotnik
{
    double tone; /* hz */
    double start; /* in seconds */
    double duration; /* in seconds */
    size_t instrument; /* ignored */
    double left_volume; /* 0-1, 1 is max */
    double right_volume; /* ^^ */

    pravokotnik(
            double tone, double start, double duration,
            size_t instrument = 0,
            double left_volume = 0.7,
            double right_volume = 0.7)
        : tone(tone)
        , start(start)
        , duration(duration)
        , instrument(instrument)
        , left_volume(left_volume)
        , right_volume(right_volume)
    {}


    bool operator>(const pravokotnik& other) const
    {
        return start > other.start;
    }

    std::string str()
    {
        std::stringstream ss;
        ss << "tone: " << tone << "hz, " << "start: " << start << ", duration: "
            << duration;
        return ss.str();
    }
};

typedef std::priority_queue<
    pravokotnik,
    std::vector<pravokotnik>,
    std::greater<pravokotnik>
    > piano_roll;

struct Sample {
    float left;
    float right;
};

struct player
{
    double time;
    piano_roll roll;
    std::list<pravokotnik> active;

    player(piano_roll roll)
        : time(0)
        , roll(roll)
        , active()
    {}

    void advance(double dt)
    {
        assert((dt >= 0) && "Not meant to go backwards!");
        time += dt;

        // add all instruments that have to sound at this time
        if (!roll.empty()) {
            auto vrh = roll.top();
            if (vrh.start <= time) {
                active.push_back(vrh);
                roll.pop();
            }
        }

        // remove all instruments that are to be removed.
        auto p = active.begin();
        auto e = active.end();
        while (p != e) {
            if (p->start + p->duration < time) {
                p = active.erase(p);
            } else {
                ++p;
            }
        }
    }

    Sample sound()
    {
        double normalize = 1/volume_norm();
        Sample s = {0, 0};
        for (auto p : active) {
            auto a = sin(2 * M_PI * p.tone * time);
            s.left  += a * p.left_volume  * normalize;
            s.right += a * p.right_volume * normalize;
        }
        return s;
    }

    double volume_norm()
    {
        double left = 0;
        double right = 0;
        for (auto p : active) {
            left  += p.left_volume;
            right += p.right_volume;
        }
        if (left < 1 ) {left = 1; }
        if (right < 1) {right = 1;}
        return std::max(left, right);
    }
};

double dist(double in) {
    double sign = (in > 0)? 1 : -1;
    in *= sign;
    return sign * (3.08095 * in - 2.38095 * in * in);
//    return sign * (2.08095 * in - 1.38095 * in * in);
}

void audio_callback(void *userdata, Uint8 *stream_in, int len_in);

struct callback_data {
    player track;
    SDL_AudioSpec fmt;
    std::vector<Sample> sound;

    callback_data(player track, size_t samples)
        : track(track)
        , sound()
        , fmt()
    {
        fmt.callback = audio_callback;
        fmt.channels = 2;
        fmt.format   = AUDIO_S16;
        fmt.samples  = 2*samples;
        fmt.freq     = 44100;
        fmt.userdata = this;

        SDL_AudioSpec got_fmt;
        SDL_OpenAudio(&fmt, &got_fmt);

        fmt = got_fmt;
        sound.resize(fmt.samples);
    }
};

void fmtdataToStream(const callback_data& data, Uint8 *stream, int len);
void audio_callback(void *userdata, Uint8 *stream_in, int len_in) {
    if (userdata == NULL) { return; }

    callback_data &fmt_data = *static_cast<callback_data*>(userdata);

    double samples_per_sec = fmt_data.fmt.freq;
    double dt_per_tick = 1/static_cast<double>(samples_per_sec);

    size_t end = fmt_data.sound.size();
    for (int tick = 0; tick < end; ++tick) {
        fmt_data.track.advance(dt_per_tick);
        fmt_data.sound[tick] = fmt_data.track.sound();
    }
    fmtdataToStream(fmt_data, stream_in, len_in);
}

void fmtdataToStream(const callback_data& data, Uint8 *stream, int len)
{
    size_t end = data.sound.size();
    for (size_t i = 0; i < end; ++i) {
        int16_t right = data.sound[i].right * 0x7FFF;
        int16_t left = data.sound[i].left * 0x7FFF;

        reinterpret_cast<int16_t*>(stream)[i*2] = left;
        reinterpret_cast<int16_t*>(stream)[i*2+1] = right;
    }
}



int main( int argc, char *argv[] )
{

    piano_roll pr;
    pr.push(pravokotnik(440  , 0    , 2));
    pr.push(pravokotnik(880  , 1    , 1));
    pr.push(pravokotnik(1760 , 1.5  , 0.5));
    pr.push(pravokotnik(3520 , 1.75 , 0.25));

    callback_data data(pr, 512);

    std::cout << "channels: " << (int) data.fmt.channels << " samples: " <<
        data.fmt.samples << " freq: " << data.fmt.freq << std::endl;

    SDL_PauseAudio(0);

    while (true) {
        usleep(1600);
    }

    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
