/**
 * @file sndgraph.cpp
 * Main file for the sngraph app.
 *
 * @author Gašper Ažman (GA), gasper.azman@gmail.com
 * @version 1.0
 * @since 2010-06-03 03:50:57 PM
 */

#include "player.hpp"
#include "composition.hpp"

#include <SDL/SDL.h>
#include <SDL/SDL_audio.h>

#include <cstdlib>

#include <iostream>
#include <unistd.h>

using namespace sgr;
using namespace std;

double dist(double in) {
    double sign = (in > 0)? 1 : -1;
    in *= sign;
    return sign * (3.08095 * in - 2.38095 * in * in);
//    return sign * (2.08095 * in - 1.38095 * in * in);
}

void audio_callback(void *userdata, Uint8 *stream_in, int len_in);

struct callback_data {
    sgr::player::player track;
    SDL_AudioSpec fmt;
    vector<sgr::player::sample> sound;

    callback_data(sgr::player::player track, size_t samples)
        : track(track)
        , fmt()
        , sound()
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

void fmtdataToStream(
        const callback_data& data,
        Uint8 *stream,
        size_t stream_len
        )
{
    size_t end = data.sound.size();
    for (size_t i = 0; (i < end) && (2*i+1 < stream_len); ++i) {
        int16_t right = data.sound[i].right * 0x7FFF;
        int16_t left = data.sound[i].left * 0x7FFF;

        reinterpret_cast<int16_t*>(stream)[i*2] = left;
        reinterpret_cast<int16_t*>(stream)[i*2+1] = right;
    }
}

void audio_callback(void *userdata, Uint8 *stream_in, int len_in) {
    if (userdata == NULL) { return; }

    callback_data &fmt_data = *static_cast<callback_data*>(userdata);

    double samples_per_sec = fmt_data.fmt.freq;
    double dt_per_tick = 1/static_cast<double>(samples_per_sec);

    size_t end = fmt_data.sound.size();
    for (size_t tick = 0; tick < end; ++tick) {
        fmt_data.track.advance(dt_per_tick);
        fmt_data.sound[tick] = fmt_data.track.sound();
    }
    fmtdataToStream(fmt_data, stream_in, len_in);
}


int main( int /* argc */, char ** /* argv */ )
{
    using namespace sgr::notation;
    try {

    sgr::notation::song song;

    sgr::composition::resources stuff;

    song << timing::linear::create(200, 0.5, 50);

    for (size_t i = 0; i < 200; i+=4) {
        song << note(
                    instrument::sinewave::create(),
                    volume::fade::create(0,0,0.7,0.7),
                    pitch::constant::create(0),
                    hit(i,2,1)
                    )
            << note(
                    instrument::sinewave::create(),
                    volume::simple::create(0.4,0.5),
                    pitch::constant::create(-3),
                    hit(i+2,2,1)
                    );
    }

    callback_data data( sgr::player::player(song), 512);

    std::cout << "channels: " << (int) data.fmt.channels << " samples: " <<
        data.fmt.samples << " freq: " << data.fmt.freq << std::endl;

    SDL_PauseAudio(0);

    while (true) {
        usleep(1600);
    }

    } catch ( boost::exception& e) {
        std::cerr << boost::diagnostic_information(e);
    }
    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
