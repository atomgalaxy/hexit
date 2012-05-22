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
    vector<scalars::sample> sound;

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

    // cast frequency from int to double
    units::frequency samples_per_sec{static_cast<double>(fmt_data.fmt.freq)};
    units::time dt_per_tick{1/samples_per_sec.value};

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
    using units::scale_offset;
    namespace sc = scalars;

    try {

    const auto number_of_bars = 64;
    const auto beats_per_bar  = 3;
    const auto bars_per_chord = 4;

    sgr::notation::song song;

    sgr::composition::resources stuff;

    auto lestvica = stuff.scales()["ionian"];
    auto trozvok  = stuff.chords()["trichord"];
    auto progression = stuff.progressions()["full circle"];
    auto bass_basenote = units::tone{-31};
    auto mid_basenote  = units::tone{-19};
    auto high_basenote = units::tone{3};

    auto chords    = lestvica.progression(progression, trozvok);
    auto bass_toni = bass_basenote + chords;
    auto mid_toni  = mid_basenote  + chords;
    auto high_toni = high_basenote + chords;

    auto passages_cnt = number_of_bars/(bars_per_chord*bass_toni.size());
    for (auto i = 0; i < passages_cnt; ++i) {
        for (auto j = 0; j < bass_toni.size(); ++j) { // generate all chords
            auto phrase_length = units::beat{beats_per_bar * bars_per_chord};
            sgr::notation::song phrase;
            phrase << timing::constant::create(phrase_length, units::bps{4});

            auto bass_ritem = sgr::composition::waltzbeat_bass(phrase_length);
            auto mid_ritem  = sgr::composition::waltzbeat_mid(phrase_length);
            auto high_ritem = sgr::composition::waltzbeat_mid(phrase_length);

            auto bass_note = sgr::composition::make_melody(bass_ritem, bass_toni[j]);
            phrase << bass_note;

            auto mid_note = sgr::composition::make_melody(mid_ritem, mid_toni[j]);
            phrase << mid_note;

            auto high_note = sgr::composition::make_melody(high_ritem, high_toni[j]);
            phrase << high_note;

            song << phrase;
        }
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
