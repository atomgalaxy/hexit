#ifndef SNDGRAPH_HPP
#define SNDGRAPH_HPP
/**
 * @file sndgraph.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-04
 */

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

namespace math {

double pmod(double a, double b)
{
    return (a/b - floor(a/b));
}

double mod(double a, double b)
{
    return (a/b - floor(a/b)) * b;
}

} /* end namespace math */

namespace notation {

namespace vol {

struct simple;
struct envelope_visitor {
    virtual void visit(const simple& s) = 0;
};

struct envelope : public std::enable_shared_from_this<envelope> {
    virtual void accept(envelope_visitor& vis) const = 0;
};

struct simple : public envelope {
    simple(double vol_left=0.7, double vol_right=0.7)
        : left(vol_left)
        , right(vol_right)
    {}

    double left;
    double right;

    /// visitor implementation.
    void accept(envelope_visitor& v) const { v.visit(*this); }
};

} /* end namespace vol */


namespace instruments {

struct sinewave;
struct sawwave;
struct squarewave;

struct instrument_visitor
{
    virtual void visit(const sinewave& s) = 0;
    virtual void visit(const sawwave& s) = 0;
    virtual void visit(const squarewave& s) = 0;
};

struct instrument : public std::enable_shared_from_this<instrument> {
    /// principal frequency, in halfnotes from a440
    virtual double pitch() const = 0;
    /// the start of the note, in seconds.
    virtual double start() const = 0;
    /// the end of the note, in seconds.
    virtual double end() const = 0;

    /// returns the volume envelope
    virtual std::shared_ptr<const vol::envelope> envelope() const = 0;

    /// visitor declaration
    virtual void accept(instrument_visitor& vis) const = 0;
};

struct sinewave : public instrument
{
public:
    sinewave(double pitch_, double start_, double duration_,
            std::shared_ptr<const vol::envelope> envelope_)
        : pitch_(pitch_)
        , start_(start_)
        , end_(start_ + duration_)
        , envelope_(envelope_)
    {}
    virtual double pitch() const { return pitch_; }
    virtual double start() const { return start_; }
    virtual double end() const   { return end_; }
    virtual std::shared_ptr<const  vol::envelope> envelope() const
    {
        return envelope_;
    }
    virtual void accept(instrument_visitor& vis) const
    {
        vis.visit(*this);
    }
private:
    double pitch_;
    double start_;
    double end_;
    std::shared_ptr<const vol::envelope> envelope_;
};

struct sawwave : public instrument
{
public:
    sawwave(double pitch_, double start_, double duration_,
            std::shared_ptr<const vol::envelope> envelope_)
        : pitch_(pitch_)
        , start_(start_)
        , end_(start_ + duration_)
        , envelope_(envelope_)
    {}
    virtual double pitch() const { return pitch_; }
    virtual double start() const { return start_; }
    virtual double end() const   { return end_; }
    virtual std::shared_ptr<const vol::envelope> envelope() const
    {
        return envelope_;
    }
    virtual void accept(instrument_visitor& vis) const
    {
        vis.visit(*this);
    }
private:
    double pitch_;
    double start_;
    double end_;
    std::shared_ptr<const vol::envelope> envelope_;
};

struct squarewave : public instrument
{
public:
    squarewave(double pitch_, double start_, double duration_,
            std::shared_ptr<const vol::envelope> envelope_)
        : pitch_(pitch_)
        , start_(start_)
        , end_(start_ + duration_)
        , envelope_(envelope_)
    {}
    virtual double pitch() const { return pitch_; }
    virtual double start() const { return start_; }
    virtual double end() const   { return end_; }
    virtual std::shared_ptr<const  vol::envelope> envelope() const
    {
        return envelope_;
    }
    virtual void accept(instrument_visitor& vis) const
    {
        vis.visit(*this);
    }
private:
    double pitch_;
    double start_;
    double end_;
    std::shared_ptr<const vol::envelope> envelope_;
};

} /* end namespace instruments */

struct song {
public:
    typedef instruments::instrument note_t;
    /* LISTENER PATTERN */
    struct note_listener {
        virtual void notify(const std::shared_ptr<const note_t> note) = 0;
    };
private:
    /// notes
    std::vector<std::shared_ptr<const instruments::instrument>> notes;

    /// listeners
    std::set<std::shared_ptr<note_listener>> listeners;

public:

    void listen(std::shared_ptr<note_listener> listener) {
        listeners.insert(listener);
    }

    void remove_listener(std::shared_ptr<note_listener> listener) {
        listeners.erase(listener);
    }

    /* actual song functions */
    void add_note(const std::shared_ptr<const note_t> note)
    {
        notes.push_back(note);
        for (auto listener : listeners) {
            listener->notify(note);
        }
    }

    const
    std::vector<std::shared_ptr<const note_t>>
    active_notes(double time)
    {
        std::vector<std::shared_ptr<const note_t>> instrs;
        for (auto note : notes) {
            if (note->start() >= time && note->end() <= time) {
                instrs.push_back(note);
            }
        }
    }

    const decltype(notes)& get_song()
    {
        return notes;
    }

};

} /* end namespace notation */

namespace composition {

class scales
{
public:
    // diatonic scales
    // major scales
    std::vector<double> ionian; // normal major (1) (C triad)
    std::vector<double> lydian; // (4)
    std::vector<double> mixolydian; // (5)
    // minor scales
    std::vector<double> dorian;   // (2) (Dm triad)
    std::vector<double> phrygian; // (3) (Em triad)
    std::vector<double> aeolian; // normal minor (6)
    // diminished
    std::vector<double> locrian; // (7)

    // others
    std::vector<double> pentatonic_minor;
    std::vector<double> pentatonic_major;
    std::vector<double> pentatonic_egyptian;
    std::vector<double> pentatonic_blues_major;
    std::vector<double> pentatonic_blues_minor;

    std::vector<double> harmonic_minor;
    std::vector<double> melodic_minor;
    std::vector<double> hungarian_minor;

    std::vector<double> double_harmonic;
    std::vector<double> arabic_harmonic; // quater-tone version
    std::vector<double> hungarian_gypsy;

    scales()
    {
        using namespace boost::assign; // for operator+=() on vector
        // diatonics
        ionian    += 0,2,4,5,7,9,11;
        dorian     = mode(ionian, 2);
        phrygian   = mode(ionian, 3);
        lydian     = mode(ionian, 4);
        mixolydian = mode(ionian, 5);
        aeolian    = mode(ionian, 6);
        locrian    = mode(ionian, 7);

        // variations on the minor
        //                 1 2 3 4 5 6 7
        //                 C D E F G A H
        // ionian:         0 2 4 5 7 9 11
        // aeolian:        0 2 3 5 7 8 10
        harmonic_minor  += 0,2,3,5,7,8,11;
        hungarian_minor += 0,2,3,6,7,8,11;
        melodic_minor   += 0,2,3,5,7,9,11;
        double_harmonic += 0,1,3,4,7,8,11;
        arabic_harmonic += 0,0.5,3,4,7,8,11.5;
        hungarian_gypsy  = mode(double_harmonic, 4);

        pentatonic_minor      += 0,3,5,7,10;
        pentatonic_major       = mode(pentatonic_minor, 2);
        pentatonic_egyptian    = mode(pentatonic_minor, 3);
        pentatonic_blues_major = mode(pentatonic_minor, 4);
        pentatonic_blues_minor = mode(pentatonic_minor, 5);
    }

    std::vector<double>
    mode(const std::vector<double>& scale, size_t n)
    {
        using math::mod;
        n -= 1;
        size_t size = scale.size();
        double first = scale[n % size];
        std::vector<double> newscale(size);
        for (size_t i = 0; i < size; ++i) {
            newscale[i] = mod((scale[(n+i)%size]-first + 12), 12);
        }
        return newscale;
    }
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
    virtual sample volume(double t, double dt)
    {
        using std::min;
        // decrackle
        double edge_dist = min(min(t - start, end - t) / GLOBAL_FALLOFF, 1.0);
        return sample(data.left * edge_dist, data.right * edge_dist);
    }

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
}; /*  end struct instruction */

struct sine : public instruction
{
    sine(const notation::instruments::sinewave& data)
        : instruction(data)
        , freq(util::tone_to_freq(data.pitch()))
    {}

    virtual sample play(double t, double dt)
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

    virtual sample play(double t, double dt)
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
    std::shared_ptr<vol::envelope> envelope;
};

struct squarewave : public instruction
{
    squarewave(const notation::instruments::squarewave& data)
        : instruction(data)
        , freq(util::tone_to_freq(data.pitch()))
    {}

    virtual sample play(double t, double dt)
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
    std::shared_ptr<vol::envelope> envelope;
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

    double start;
    double stop;
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
 * //  make piano roll
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
