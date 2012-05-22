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
#include "units.hpp"
#include "../misc/utility.hpp"

#include <boost/assign/std/vector.hpp>
#include <utility>
#include <map>

namespace sgr {
namespace composition {

class scale {
    std::vector<units::interval> offsets;

public:
    scale()
        : offsets() {}

    scale(std::vector<units::interval> offsets_)
        : offsets(offsets_)
    {}

    /**
     * Get an interval from a scale.
     *
     * @param interval the offset within the scale. If you want the basenote,
     * put in 0. If you want the second note, put in 1. If you want the last
     * note, but one octave down, put in -1.
     *
     * @return the interval that this scale offset represents.
     */
    units::interval
    interval(const units::scale_offset& offset) const
    {
        auto n = offsets.size();
        int octaves = offset.value / n;
        int r = offset.value % n;

        if (r < 0) {
            r += n;
            octaves -= 1;
        }

        return units::interval{octaves, offsets[r]};
    }

    /**
     * Accessor for offsets - provides a periodic view of the scale.
     * Negative offsets will go from the end of the scale, positive offsets from
     * the beginning.
     */
    units::scale_offset
    offset(const units::scale_offset& offset) const
    {
        int o = offset.value % int(offsets.size());
        if (o < 0) { o += offsets.size(); }
        return units::scale_offset{o};
    }

    /**
     * Get intervals from a chord.
     *
     * @param ch the chord to convert to intervals.
     * @return the intervals.
     */
    units::intervals_type
    intervals(const units::chord& ch) const
    {
        units::intervals_type t;
        for (auto i : ch) {
            t.emplace_back(interval(i));
        }
        return t;
    }

    std::vector<units::intervals_type>
    progression(units::chord progression, units::chord basechord) const
    {
        std::vector<units::intervals_type> r;
        for (auto i : progression) {
            r.push_back(intervals(i + basechord));
        }
        return r;
    }

    size_t size() const { return offsets.size(); }

};

/**
 * Returns the nth mode of the scale sc.
 */
scale mode(const scale& sc, const units::scale_offset& n)
{
    using namespace units;

    auto first = sc.interval(n);
    auto size = sc.size();
    intervals_type newscale;

    for (unsigned int i = 0; i < size; ++i) {
        newscale.emplace_back(sc.interval(n + units::scale_offset{(int)i}) - first);
    }

    return scale{std::move(newscale)};
}

typedef std::vector<sgr::notation::hit> rhythm;

rhythm
waltzbeat_bass(units::beat biti)
{
    using namespace sgr::notation;
    using utility::rand;

    rhythm bass;
    for (int i = 0; i < biti.value; ++i) {
        double poudarek;
        if (i%3 == 0) {
            poudarek = 1;
        } else {
            poudarek = 0.7;
        }
        bass.push_back(hit(units::beat{double(i)}, units::beat{1}, poudarek));
    }

    for (int i = 0; i < 6; ++i) {
        int a = rand(0, 101);
        if (a < 35) {
          for (int j = 0; j < (biti/3).value; ++j) {
              bass.push_back(
                      hit(
                          units::beat{double(i)/2 + j*3},
                          units::beat{0.5},
                          0.4));
            }
        }
    }
    return bass;
}

/// TODO write the damn thing
rhythm
waltzbeat_mid(units::beat biti)
{
    using namespace sgr::notation;

    std::vector<sgr::notation::hit> mid;
    return mid;
}

/// TODO write the damn thing
rhythm
waltzbeat_high(units::beat biti)
{
    using namespace sgr::notation;

    std::vector<sgr::notation::hit> high;
    return high;
}

std::vector<rhythm>
waltzbeat(units::beat beati)
{
    using namespace sgr::notation;

    std::vector<rhythm> rhythm;

    rhythm.push_back(waltzbeat_bass(beati));
    rhythm.push_back(waltzbeat_mid(beati));
    rhythm.push_back(waltzbeat_high(beati));
    return rhythm;
}

/**
 * Makes a melody out of a set of tones and a rhythm.
 * @param hits the sequence of notes to generate a melody to
 * @param tones the different tones we can use to generate a melody
 * @return a sequence of notes.
 */
std::vector<sgr::notation::note>
make_melody(
        const std::vector<sgr::notation::hit>& hits,
        const std::vector<units::tone>& tones
        )
{
    using namespace notation;
    using utility::random_pick;

    std::vector<sgr::notation::note> notes;
    for (auto i : hits) {
        auto tone = random_pick(tones);
        notes.emplace_back(
                instrument::sinewave::create(),
                volume::fade::create(scalars::volume{0.8,0.8}, scalars::volume{0.4,0.4}),
                pitch::constant::create(tone),
                i
        );
    }
    return notes;
}

//namespace rhythm {
////**
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

std::vector<units::interval>
to_intervals(const std::initializer_list<double>& offs) {
    std::vector<units::interval> sc;
    for (auto n : offs) {
        sc.emplace_back(n);
    }
    return sc;
}

class resources
{
private:
    std::map<std::string, scale> scales_;
    std::map<std::string, units::chord> chords_;
    std::map<std::string, units::chord> progressions_;
public:

    resources()
        : scales_()
        , chords_()
    {
        typedef std::vector<double> sc_t;
        using units::scale_offset;

        using namespace boost::assign; // for operator+=() on vector
        // diatonics
        scales_["ionian"]     = scale(to_intervals({0.,2.,4.,5.,7.,9.,11.}));
        scales_["dorian"]     = mode(scales_["ionian"], scale_offset{2});
        scales_["phrygian"]   = mode(scales_["ionian"], scale_offset{3});
        scales_["lydian"]     = mode(scales_["ionian"], scale_offset{4});
        scales_["mixolydian"] = mode(scales_["ionian"], scale_offset{5});
        scales_["aeolian"]    = mode(scales_["ionian"], scale_offset{6});
        scales_["locrian"]    = mode(scales_["ionian"], scale_offset{7});

        // variations on the minor
        //                 1 2 3 4 5 6 7
        //                 C D E F G A H
        // ionian:         0 2 4 5 7 9 11
        // aeolian:        0 2 3 5 7 8 10
        scales_["harmonic minor"]  = scale(to_intervals({0.,2.,3.,5.,7.,8.,11.}));
        scales_["hungarian minor"] = scale(to_intervals({0.,2.,3.,6.,7.,8.,11.}));
        scales_["melodic minor"]   = scale(to_intervals({0.,2.,3.,5.,7.,9.,11.}));
        scales_["double harmonic"] = scale(to_intervals({0.,1.,3.,4.,7.,8.,11.}));
        scales_["arabic harmonic"] = scale(to_intervals({0.,0.5,3.,4.,7.,8.,11.5}));
        scales_["hungarian gypsy"] = mode(scales_["double harmonic"], scale_offset{4});

        scales_["pentatonic minor"] = scale(to_intervals({0.,3.,5.,7.,10.}));
        scales_["pentatonic major"] = mode(scales_["pentatonic minor"], scale_offset{2});
        scales_["pentatonic egyptian"] = mode(scales_["pentatonic minor"], scale_offset{3});
        scales_["pentatonic blues major"]= mode(scales_["pentatonic minor"], scale_offset{4});
        scales_["pentatonic blues minor"]= mode(scales_["pentatonic minor"], scale_offset{5});

        chords_["trichord"] = units::chord({scale_offset{0}, scale_offset{2}, scale_offset{4}});
        
        progressions_["punk"] = units::chord({scale_offset{0}, scale_offset{3}, scale_offset{4}, scale_offset{0}});
    }

    inline
    decltype(scales_)&
    scales() { return scales_; }

    inline
    decltype(chords_)&
    chords() { return chords_; }

    inline
    decltype(progressions_)&
    progressions() { return progressions_; }

};

}/* end namespace composition */
}/* end namespace sgr */

#endif
