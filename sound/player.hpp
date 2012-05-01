#ifndef SGR_PLAYER_HPP
#define SGR_PLAYER_HPP
/**
 * @file player.hpp
 * Contains everything required to play the sgr::notation::song instances.
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */

#include "math.hpp"
#include "notation.hpp"
#include "player_instrument.hpp"
#include "player_timing.hpp"
#include "scalars.hpp"
#include "player_pitch.hpp"
#include "player_volume.hpp"

#include "exceptions.hpp"

#include <memory>
#include <queue>
#include <vector>
#include <list>
#include <cassert>

#include <iostream> //for cerr
#include <sstream>

namespace sgr {
namespace player {

struct instruction {
    typedef std::shared_ptr<const instruction> pointer_type;

    pitch::pitch::pointer_type           pitch;
    instrument::instrument::pointer_type instrument;
    volume::volume::pointer_type         volume;
    timing::period                       bounds;

    instruction(const notation::note& n, units::time start_t, units::time end_t)
        : pitch{pitch::factory(*n.pitch)}
        , instrument{instrument::factory(*n.instrument)}
        , volume{volume::factory(*n.volume)}
        , bounds{timing::time(start_t, n.duration.start),
                 timing::time(end_t, n.duration.start + n.duration.duration)}
    {}

    std::string str() {
        std::stringstream ss;
        ss << "Instrument: " << instrument->str() << std::endl;
        ss << "Pitch: " << pitch->str() << std::endl;
        ss << "Volume: " << volume->str() << std::endl;
        ss << "Period: " << bounds.str() << std::endl;
        return ss.str();
    }
};

struct tempo {
    typedef std::list<notation::timing::timing::pointer_type> timing_queue;

    private:
    timing_queue timings;
    timing::time now;
    timing::time global;

    public:

    tempo()
        : timings()
        , now(units::time{0},units::beat{0},units::time{0},units::beat{0})
        , global(units::time{0},units::beat{0},units::time{0},units::beat{0}) {}

    void add_timing(const notation::timing::timing::pointer_type timing)
    {
        timings.push_back(timing);
    }

    void advance(units::time dt) throw (err::end_of_song)
    {
        using units::time;
        assert((dt >= time{0}) && "must be greater than zero!");
        timing::time old(now);

        global.t += dt;
        global.dt = dt;

        now.t += dt;
        now.dt = dt;

        auto t = timings.front();
        if (t->full_time() <= now.t) { /* if finished with timing sequence */
            /* switch the timing sequence */

            /* remember how much time and beat remained */
            auto rest_of_t = t->full_time() - old.t;
            auto rest_of_b = t->duration - old.beat;

            now.t = dt - rest_of_t; /* current time is dt - remaining */
            old.beat = -rest_of_b; /* dbeat = now.beat - old.beat */

            /* change the timing */
            timings.pop_front();
            if (timings.size() == 0) {
                throw err::end_of_song()
                    << err::reason("Ran out of timing specs!");
            }
            t = timings.front();
        }
        now.beat = t->time_to_beat(now.t);
        now.dbeat = now.beat - old.beat;

        global.beat += now.dbeat;
        global.dbeat = now.dbeat;
//        std::cout << "dt: " << dt.value << " dbeat: " << now.dbeat.value << "\n";
    }

    /** Returns the time of a specified beat.
     * Relative to beginning of song.
     * DO NOT USE for beat that have already passed!
     */
    units::time beat_to_time(units::beat beat) throw (sgr::err::invalid_beat)
    {
        using namespace sgr::err;

        if (beat < global.beat) {
            throw invalid_beat() << beat_val(beat.value)
                << reason("Beat in the past.");
        }
        auto t = global.t;
        beat -= global.beat - now.beat;
        for (auto l : timings) {
//            std::cerr << l->full_time() << std::endl;
            if (beat <= l->duration) {
                return t + l->beat_to_time(beat);
            } else {
                t += l->full_time();
                beat -= l->duration;
            }
        }
        throw invalid_beat() << beat_val(beat.value) <<
            reason("Beat past end of song.");
    }

    const timing::time& get() { return global; }
};

/**
 * A class for playing piano rolls.
 * Use like so:
 * //  create piano roll
 * player pl(music);
 * while(true) {
 *  sample s = pl.sound();
 *  pl.advance(0.001);
 * }
 */
class player
{
    struct ptrcmp {
        bool operator()(
                const instruction& a,
                const instruction& b
        ) {
            return a.bounds.start.beat > b.bounds.start.beat;
        }
    };

    typedef std::priority_queue<
        instruction,
        std::vector<instruction>,
        ptrcmp
        > piano_roll;

    tempo  now;
    piano_roll roll;
    std::list<instruction> active;

    scalars::sample sound_;
public:
    player(notation::song song)
        : now()
        , roll()
        , active()
        , sound_()
    {
        for (auto timing : song.timings()) {
            now.add_timing(timing);
        }
        for (auto instr : song.notes()) {
            auto start_t = now.beat_to_time(instr.duration.start);
            auto end_t   = now.beat_to_time(instr.duration.start + instr.duration.duration);
            roll.push(instruction(instr, start_t , end_t));
        }
    }

    /**
     * Advances the composition by dt seconds.
     * @param dt the time to advance, in seconds. Must be >= 0.
     */
    void advance(units::time dt)
    {
        using std::max;
        assert((dt >= units::time{0}) && "Not meant to go backwards!");

        now.advance(dt);
        auto t = now.get();

        // add all instruments that have to sound at this t
        if (!roll.empty()) {
            auto top = roll.top();
            if (top.bounds.start.beat <= t.beat) {
                std::cout << "Beat: " << t.beat.value << ". Adding " <<
                    top.str();
                active.push_back(top);
                roll.pop();
            }
        }

        // remove all instructions that are to be removed.
        auto p = active.begin();
        auto e = active.end();
        while (p != e) {
            if (p->bounds.end.beat < t.beat) {
//                std::cout << "Beat: " << t.beat.value << ". removing " <<
//                    p->str();
                p = active.erase(p);
            } else {
                ++p;
            }
        }

        // compute sound
        double normalize = 0;
        scalars::sample s{0, 0};
        for (auto p : active) {
//            std::cout << p.str();
            auto vol = p.volume->get_volume(p.bounds, t);
            auto pitch = p.pitch->get_pitch(p.bounds, t);
            s += p.instrument->get_sample(p.bounds, t, vol, pitch);
            normalize += max(vol.right, vol.left);
        }
        normalize = max(normalize, 1.0);
        s *= scalars::volume{1/normalize, 1/normalize};
        sound_ = s;
    }

    /**
     * Extract the current soundsample.
     * @return the current sound amplitude.
     */
    scalars::sample sound()
    {
        return sound_;
    }
}; /* end struct player */
} /* end namespace player */
}/* end namespace sgr */

#endif
