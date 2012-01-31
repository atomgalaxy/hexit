#ifndef SNDGRAPH_HPP
#define SNDGRAPH_HPP
/**
 * @file sndgraph.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-04
 */
// local includes
#include "framework.hpp"
#include "math.hpp"

// global includes
#include <cstddef> // for size_t
#include <cassert> // for assert
#include <cmath>   // for sin, cos, M_PI

#include <string>  // for std::string
#include <sstream> // for std::stringstream
#include <queue>   // for priority_queue and vector
#include <list>    // for std::list

//#include <boost/ptr_container/ptr_map.hpp>
#include <boost/assign/std/vector.hpp>

#include <iostream>
#include <memory>
#include <map>
#include <set>


namespace sgr {

namespace composition {

/// a chord is a series of tone-offsets relative to a scale.
/// for instance, 0, 2, 4 (1, 3, 5) is the usual chord, that, when played in a
/// major scale, gives a major chord.
typedef std::vector<int> chord;

/// tones are semitone offsets from a440.
typedef std::vector<double> tones_type;

struct scale {
    scale()
        : offsets() {}

    template<typename Arg>
    scale(Arg&& offsets)
        : offsets(std::forward<Arg>(offsets))
    {}

    scale(scale&& sc)
        : offsets(std::move(sc.offsets))
    {}

    scale mode(int n) const
    {
        using math::mod;
        n -= 1;
        size_t size = offsets.size();
        double first = offsets[n % size];
        std::vector<double> newscale(size);
        for (size_t i = 0; i < size; ++i) {
            newscale[i] = mod((offsets[(n+i)%size]-first + 12), 12);
        }
        return scale(std::move(newscale));
    }

    /**
     * Get a particular tone from a scale.
     *
     * @param basenote - the note that is considered the "base" of the scale.
     * For instance, if this scale was a major scale, and you wanted C-major,
     * you set basenote = -9. This is because basenotes are relative to a440.
     * 
     * @param interval the offset within the scale. If you want the basenote,
     * put in 0. If you want the second note, put in 1. If you want the last
     * note, but one octave down, put in -1.
     *
     * @return the tone, in halftone offsets from a440
     */
    double tone(double basenote, int interval)
    {
        size_t n = offsets.size();
        int octaves = interval / n;
        interval = interval % n;

        if (interval < 0) {
            interval   += n;
            octaves -= 1;
        }

        basenote += octaves * 12;
        return offsets[interval] + basenote;
    }

    /**
     * Get tones from a chord.
     *
     * @param basenote - the note that is considered the "base" of the scale.
     * For instance, if this scale was a major scale, and you wanted C-major,
     * you set basenote = -9. This is because basenotes are relative to a440.
     *
     * @param ch the chord to convert to notes.
     *
     * @return the tones, in halfnote offsets from a440.
     */
    tones_type
    tones(double basenote, const chord& ch)
    {
        std::vector<double> t;
        for (auto i : ch) {
            t.push_back(tone(basenote, i));
        }
        return t;
    }

private:
    std::vector<double> offsets;
};


class resources
{
private:
    std::map<std::string, scale> scales_;
    std::map<std::string, chord> chords_;
public:

    resources()
        : scales_()
        , chords_()
    {
        typedef std::vector<double> sc_t;
        using namespace boost::assign; // for operator+=() on vector
        // diatonics
        scales_["ionian"]     = scale(sc_t{0,2,4,5,7,9,11});
        scales_["dorian"]     = scales_["ionian"].mode(2);
        scales_["phrygian"]   = scales_["ionian"].mode(3);
        scales_["lydian"]     = scales_["ionian"].mode(4);
        scales_["mixolydian"] = scales_["ionian"].mode(5);
        scales_["aeolian"]    = scales_["ionian"].mode(6);
        scales_["locrian"]    = scales_["ionian"].mode(7);

        // variations on the minor
        //                 1 2 3 4 5 6 7
        //                 C D E F G A H
        // ionian:         0 2 4 5 7 9 11
        // aeolian:        0 2 3 5 7 8 10
        scales_["harmonic minor"]  = scale(sc_t{0,2,3,5,7,8,11});
        scales_["hungarian minor"] = scale(sc_t{0,2,3,6,7,8,11});
        scales_["melodic minor"]   = scale(sc_t{0,2,3,5,7,9,11});
        scales_["double harmonic"] = scale(sc_t{0,1,3,4,7,8,11});
        scales_["arabic harmonic"] = scale(sc_t{0,0.5,3,4,7,8,11.5});
        scales_["hungarian gypsy"] = scales_["double harmonic"].mode(4);

        scales_["pentatonic minor"] = scale(sc_t{0,3,5,7,10});
        scales_["pentatonic major"] = scales_["pentatonic minor"].mode(2);
        scales_["pentatonic egyptian"] = scales_["pentatonic minor"].mode(3);
        scales_["pentatonic blues major"]=scales_["pentatonic minor"].mode(4);
        scales_["pentatonic blues minor"]=scales_["pentatonic minor"].mode(5);

        chords_["trichord"] = chord({0,2,4});
    }

    inline
    const std::map<std::string, scale>&
    scales() { return scales_; }

    inline
    const std::map<std::string, chord>&
    chords() { return chords_; }

};


} /* end namespace composition */

namespace player {

namespace util {
    static const double TAU = 2 * M_PI;
    static const double TWELFTH_ROOT_OF_2 = 1.05946309435929;

    /**
     * Returns the frequency of the given tone.
     * @param tone the offset, in semitones, from a440. Negative values are of
     * course accepted.
     */
    double tone_to_freq(double tone)
    {
        double shift = pow(TWELFTH_ROOT_OF_2, tone);
        return 440 * shift;
    }
} /* end namespace player::util */

struct sample
{
    sample(double left = 0, double right = 0)
        : left(left)
        , right(right)
    {}

    double left;
    double right;
}; /* end struct sample */

namespace vol {

struct envelope
{
    constexpr static double GLOBAL_FALLOFF = 0.02;
    virtual sample volume(double t, double dt) = 0;
    virtual ~envelope() {}
};

struct simple : public envelope
{
    simple( const notation::vol::simple& data,
            double start,
            double end
            )
        : data(data)
        , start(start)
        , end(end)
    {}
    virtual sample volume(double t, double /* dt */)
    {
        using std::min;
        // decrackle
        double edge_dist = min(min(t - start, end - t) / GLOBAL_FALLOFF, 1.0);
        return sample(data.left * edge_dist, data.right * edge_dist);
    }
    virtual ~simple() {}

    notation::vol::simple data;
    double start;
    double end;
};

struct factory_visitor : public notation::vol::envelope_visitor
{
    typedef std::shared_ptr<envelope> env_ptr_t;

    factory_visitor(double start, double stop)
        : start(start)
        , stop(stop)
        , obj(nullptr)
    {}

    void visit(const notation::vol::simple& env)
    {
        obj = env_ptr_t(new simple(env, start, stop));
    }

    std::shared_ptr<envelope>
    getobj()
    {
        return obj;
    }

    double start;
    double stop;
    env_ptr_t obj;
};

std::shared_ptr<envelope>
factory(
        const notation::instruments::instrument& instr
        )
{
    factory_visitor v(instr.start(), instr.end());
    instr.envelope()->accept(v);
    return v.getobj();
}

} /* end namespace vol */

namespace instructions {

struct instruction
{
    instruction(const notation::instruments::instrument& data)
        : start(data.start())
        , end(data.end())
        , envelope(vol::factory(data))
    {}

    const double start;
    const double end;
    std::shared_ptr<vol::envelope> envelope;
    sample volume(double t, double dt) {
        return envelope->volume(t, dt);
    }

    std::string str()
    {
        std::stringstream ss;
        ss << ", start: " << start
           << ", end: " << end
           << ", duration: " << end-start
           << std::endl;
        return ss.str();
    }

    virtual sample play(double t, double dt) = 0;
    virtual ~instruction() {}
}; /*  end struct instruction */

struct sine : public instruction
{
    sine(const notation::instruments::sinewave& data)
        : instruction(data)
        , freq(util::tone_to_freq(data.pitch()))
    {}

    virtual sample play(double t, double /* dt */)
    {
        double x = util::TAU * freq * t;
        auto a = sin(x);
        return sample(a, a);
    }

    double freq;
}; /* end struct sine */

struct sawtooth : public instruction
{
    sawtooth(const notation::instruments::sawwave& data)
        : instruction(data)
        , freq(util::tone_to_freq(data.pitch()))
    {}

    sample play(double t, double /* dt */)
    {
        double wavelen = 1.0/freq;
        double rest = math::pmod(t, wavelen);

        double a = 0;
        if (rest < 0.5) { // up slope
            a = -1 + 4*rest;
        } else {
            a = 1 - 4*(rest - 0.5);
        }
        return sample(a, a);
    }

    double freq;
};

struct squarewave : public instruction
{
    squarewave(const notation::instruments::squarewave& data)
        : instruction(data)
        , freq(util::tone_to_freq(data.pitch()))
    {}

    virtual sample play(double t, double /* dt */)
    {
        double wavelen = 1.0/freq;
        double rest = math::pmod(t, wavelen);

        double a = 0;
        if (rest < 0.5) { // bottom of waveform
            a = -1;
        } else {          // top of waveform
            a = 1;
        }
        return sample(a, a);
    }

    virtual sample volume(double t, double dt)
    {
        return envelope->volume(t, dt);
    }

    double freq;
};

struct factory_visitor : public notation::instruments::instrument_visitor
{
    factory_visitor()
        : obj(nullptr)
    {}

    void visit(const notation::instruments::sinewave& env)
    {
        obj = std::shared_ptr<instruction>(new sine(env));
    }
    void visit(const notation::instruments::sawwave& env)
    {
        obj = std::shared_ptr<instruction>(new sawtooth(env));
    }
    void visit(const notation::instruments::squarewave& env)
    {
        obj = std::shared_ptr<instruction>(new squarewave(env));
    }

    std::shared_ptr<instruction>
    getobj()
    {
        return obj;
    }

    std::shared_ptr<instruction> obj;
};

std::shared_ptr<instruction>
factory(const notation::instruments::instrument& instr)
{
    factory_visitor v;
    instr.accept(v);
    return v.getobj();
}

} /* end namespace instructions */


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
                const std::shared_ptr<const instructions::instruction>& a,
                const std::shared_ptr<const instructions::instruction>& b
        ) {
            return a->start > b->start;
        }
    };
    typedef std::priority_queue<
        std::shared_ptr<instructions::instruction>,
        std::vector<std::shared_ptr<instructions::instruction>>,
        ptrcmp
        > piano_roll;

    double time;
    piano_roll roll;
    std::list<std::shared_ptr<instructions::instruction>> active;

    sample sound_;
public:
    player(notation::song song)
        : time(0)
        , roll()
        , active()
        , sound_()
    {
        for (auto instr : song.get_song()) {
            roll.push(instructions::factory(*instr));
        }
    }

    /**
     * Advances the composition by dt seconds.
     * @param dt the time to advance, in seconds. Must be >= 0.
     */
    void advance(double dt)
    {
        using std::max;
        assert((dt >= 0) && "Not meant to go backwards!");

        time += dt;

        // add all instruments that have to sound at this time
        if (!roll.empty()) {
            auto vrh = roll.top();
            if (vrh->start <= time) {
                active.push_back(vrh);
                roll.pop();
            }
        }

        // remove all instructions that are to be removed.
        auto p = active.begin();
        auto e = active.end();
        while (p != e) {
            if ((*p)->end < time) {
                p = active.erase(p);
            } else {
                ++p;
            }
        }

        // compute sound
        double normalize = 0;
        sample s(0, 0);
        for (auto p : active) {
            auto out = p->play(time, dt);
            auto vol = p->envelope->volume(time, dt);
            s.left  += out.left  * vol.left;
            s.right += out.right * vol.right;
            normalize += max(vol.right, vol.left);
        }
        normalize = max(normalize, 1.0);
        s.left /= normalize;
        s.right /= normalize;
        sound_ = s;
    }

    /**
     * Extract the current soundsample.
     * @return the current sound amplitude.
     */
    sample sound()
    {
        return sound_;
    }
}; /* end struct player */
} /* end namespace player */

} /* end namespace sgr */
#endif
