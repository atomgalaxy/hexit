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

namespace sgr {

double tone_to_freq(double tone, int octave = 4)
{
    /* make it so the a440 is 9-4 */
    const double root12_2 = 1.05946309435929;
    double tonenumber = octave * 12 + tone;
    double a440 = 4*12 + 9;
    double offset = tonenumber - a440;
    double shift = pow(root12_2, offset);
    return 440 * shift;
}

double pmod(double a, double b)
{
    return (a/b - floor(a/b));
}

double mod(double a, double b)
{
    return (a/b - floor(a/b)) * b;
}


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

struct sample
{
    sample(double left = 0, double right = 0)
        : left(left)
        , right(right)
    {}

    double left;
    double right;
}; /* end struct sample */

struct instruction
{
    double freq; /* hz */
    double start; /* in seconds */
    double end; /* in seconds */
    std::string instr; /* instrument */
    double left_volume; /* 0-1, 1 is max */
    double right_volume; /* ^^ */

    instruction( double freq, double start, double end,
          std::string instr,
          double left_volume = 0.7,
          double right_volume = 0.7
          )
        : freq(freq)
        , start(start)
        , end(end)
        , instr(instr)
        , left_volume(left_volume)
        , right_volume(right_volume)
    {}


    bool operator>(const instruction& other) const
    {
        return start > other.start;
    }

    std::string str()
    {
        std::stringstream ss;
        ss << "freq: " << freq << "hz, " << "start: " << start << " end: " <<
            end << ", duration: " << end-start;
        return ss.str();
    }
}; /*  end struct instruction */


struct instrument
{
    virtual sample play(const instruction& tone, double time) const = 0;
}; /* end struct instrument */

struct sine : public instrument
{
    virtual sample play(const instruction& tone, double time) const
    {
        auto a = sin(2 * M_PI * tone.freq * time);
        return sample(a * tone.left_volume, a * tone.right_volume);
    }
}; /* end struct sine */

struct sawtooth : public instrument
{
    virtual sample play(const instruction& tone, double time) const
    {
        double wavelen = 1.0/tone.freq;
        double rest = pmod(time, wavelen);

        double a = 0;
        if (rest < 0.5) { // up slope
            a = -1 + 4*rest;
        } else {
            a = 1 - 4*(rest - 0.5);
        }
        return sample(a * tone.left_volume, a*tone.right_volume);
    }
};

std::shared_ptr<instrument>
instrument_factory(std::string which)
{
    if (which == "sine") {
        return std::shared_ptr<instrument>(new sine());
    } else if (which == "saw") {
        return std::shared_ptr<instrument>(new sawtooth());
    } else {
        return std::shared_ptr<instrument>(new sine()); // default instrument
    }
}


typedef std::priority_queue<
    instruction,
    std::vector<instruction>,
    std::greater<instruction>
    > piano_roll;


void make_melody(
        const std::vector<double>& tones,
        double start_t, double dt, piano_roll& pr
        )
{
    double t = start_t;
    for (auto tone : tones) {
        pr.pushk(instruction(tone_to_freq(tone), t, t+dt, "sine"));
        t += dt;
    }
}

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
    double time;
    piano_roll roll;
    std::list<instruction> active;
    std::map<std::string, std::shared_ptr<instrument>> instrs;

    // makes sure that the volume envelope goes to zero as the instruction
    // starts and as it approaches the end, to avoid crackling.
    double uncrackle_volume(const instruction& inst, double time) const
    {
        const double fadeout = 0.004;
        if (time - inst.start < fadeout) {
            return (time - inst.start)/fadeout;
        } else if (inst.end - time < fadeout) {
            return (inst.end - time)/fadeout;
        } else {
            return 1;
        }
    }

    // calculates the current overall volume of instruments and
    // returns it. Also uses uncrackle_volume to figure that out, since it
    // results in crackling anyways if we don't do that.
    double volume_norm() const
    {
        double left = 0;
        double right = 0;
        for (auto p : active) {
            double decrackle = uncrackle_volume(p, time);
            left  += p.left_volume * decrackle;
            right += p.right_volume * decrackle;
        }
        if (left < 1 ) {left = 1; }
        if (right < 1) {right = 1;}
        return std::max(left, right);
    }

public:
    player(piano_roll roll)
        : time(0)
        , roll(roll)
        , active()
    {
        instrs["sine"] = instrument_factory("sine");
        instrs["saw"]  = instrument_factory("saw");
    }

    /**
     * Advances the composition by dt seconds.
     * @param dt the time to advance, in seconds. Must be >= 0.
     */
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

        // remove all instructions that are to be removed.
        auto p = active.begin();
        auto e = active.end();
        while (p != e) {
            if (p->end < time) {
                p = active.erase(p);
            } else {
                ++p;
            }
        }
    }

    /**
     * Extract the current soundsample.
     * @return the current sound amplitude.
     */
    sample sound()
    {
        double normalize = 1/volume_norm();
        sample s(0, 0);
        for (auto p : active) {
            if (instrs.count(p.instr)) { // if we have the instrument
                auto out = instrs[p.instr]->play(p, time);
                double decrackle = uncrackle_volume(p, time);
                s.left  += out.left  * normalize * decrackle;
                s.right += out.right * normalize * decrackle;
            } else {
                std::cerr << "Could not find instrument \"" << p.instr << "\""
                    << std::endl;
            }
        }
        return s;
    }
}; /* end struct player */

} /* end namespace sgr */
#endif
