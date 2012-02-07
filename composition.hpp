#ifndef SGR_COMPOSITION_HPP
#define SGR_COMPOSITION_HPP
/**
 * @file composition.hpp
 * Contains the logic to compose song instances.
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */

#include "math.hpp"

#include <boost/assign/std/vector.hpp>
#include <utility>
#include <map>

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

//namespace rhythm {
///**
// * Holds a passage of hits.
// */
//struct bar {
//    bar() : hits_(), length_(0) {}
//    bar& operator<<(const hit& h)
//    {
//        if (h.start + h.duration > length_) {
//            length_ = h.start + h.duration;
//        }
//        hits_.push_back(h);
//        return *this;
//    }

//    double length() const { return length_; }
//    const std::vector<hit>& hits() const { return hits_; }
//private:
//    std::vector<hit> hits_;
//    double length_;
//};

//} /* end namespace rhythm */


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

std::vector<sgr::notation::hit>
uniform_rhythm(double beats, double start_t, double bps, double accent)
{
    std::vector<sgr::notation::hit> v;
    double b = 0;
    double time_per_beat = 1/bps;
    while (b < beats) {
        v.push_back(hit(start_t + time_per_beat * b, b, accent));
        b+=time_per_beat;
    }
    return v;
}

}/* end namespace composition */
}/* end namespace sgr */

#endif
