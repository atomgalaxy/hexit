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
#include <unistd.h>


struct Sample {
    float left;
    float right;
};

struct Fmtdata {
    SDL_AudioSpec *fmt;
    float last_time;
    size_t bytes_per_sample;
    std::vector<Sample> sound;
};

void fmtdataToStream(const Fmtdata& data, Uint8 *stream, int len) {
    size_t end = data.sound.size();
    for (size_t i = 0; i < end; ++i) {
        int16_t right = data.sound[i].right * 0x7FFF;
        int16_t left = data.sound[i].left * 0x7FFF;

        reinterpret_cast<int16_t*>(stream)[i*2] = left;
        reinterpret_cast<int16_t*>(stream)[i*2+1] = right;
    }
}

double dist(double in) {
    double sign = (in > 0)? 1 : -1;
    in *= sign;
    return sign * (3.08095 * in - 2.38095 * in * in);
//    return sign * (2.08095 * in - 1.38095 * in * in);
}

void audio_callback(void *userdata, Uint8 *stream_in, int len_in) {
    if (userdata == NULL) { return; }
    Fmtdata &fmt_data = *static_cast<Fmtdata*>(userdata);
    int len = len_in/sizeof(int16_t);

    float samples_per_sec = fmt_data.fmt->freq;
    float dt_per_tick = 1/static_cast<float>(samples_per_sec);

    float tone_hz = 440;
    float lambda = M_PI * 2 * tone_hz;
    float t = fmt_data.last_time;

    size_t end = fmt_data.sound.size();
    for (int tick = 0; tick < end; ++tick) {
        t += dt_per_tick;
        float amplitude_l = dist(sin(lambda * t));
        float amplitude_r = dist(sin(lambda * t));
//        float amplitude_l = (sin(lambda * t));
//        float amplitude_r = (sin(lambda * t));
        fmt_data.sound[tick].left = amplitude_l;
        fmt_data.sound[tick].right = amplitude_r;
    }
    fmt_data.last_time = t;
    fmtdataToStream(fmt_data, stream_in, len_in);
}


int main( int argc, char *argv[] )
{
    SDL_AudioSpec fmt;
    SDL_AudioSpec ifmt;
    Fmtdata data;
    data.fmt = &fmt;
    data.last_time = 0;
    data.bytes_per_sample = 2;

    ifmt.callback = audio_callback;
    ifmt.channels = 2;
    ifmt.format = AUDIO_S16;
    ifmt.samples = 512;
    ifmt.freq = 44100;

    ifmt.userdata = &data;

    SDL_OpenAudio(&ifmt, &fmt);

    std::cout << "channels: " << fmt.channels << " samples: " <<
        fmt.samples << " freq: " << fmt.freq << std::endl;

    data.sound.resize(fmt.samples);
    SDL_PauseAudio(0);

    while (true) {
        usleep(1600);
    }

    return EXIT_SUCCESS;
}               /* ----------  end of function main  ---------- */
