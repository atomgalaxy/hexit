#ifndef SGR_NOTATION_TIMING_HPP
#define SGR_NOTATION_TIMING_HPP
/**
 * @file notation_timing.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

#include "math.hpp"
#include "units.hpp"
#include <memory>
#include <iostream>

namespace sgr {
namespace notation {
namespace timing {

struct timing {
    typedef std::shared_ptr<const timing> pointer_type;
    units::beat duration; ///< in beats

    timing(decltype(duration) duration) : duration(duration) {}

    /// Should return the current bps, in bps, since the start of this timing
    /// sequence
    virtual units::bps bps(units::beat beat) const = 0;

    /**
     * Given a time point (with 0 being the beginning of this timing period),
     * calculates the number of beats that have passed since the beginning of
     * this time period.
     */
    virtual units::beat time_to_beat(units::time time) const = 0;

    /**
     * Given a beat point, calculates the time that has passed in order to reach
     * that beat. Zero coresponds to beginning of this time period, in both
     * ways.
     */
    virtual units::time beat_to_time(units::beat beat) const = 0;

    /**
     * Returns the full time of the timing sequence.
     */
    virtual units::time full_time() const = 0;

    /**
     * Returns the full beat length of the timing sequence.
     */
    units::beat full_beats() const {
        return duration;
    }

    /// virtual dtor
    virtual ~timing() {}
};

/** Constant bpm timing.  */
class constant : public timing {
    units::bps bps_;
    units::time full_time_;

    constant(units::beat duration, units::bps bps)
        : timing(duration), bps_(bps), full_time_(duration / bps) {}

public:
    typedef std::shared_ptr<const constant> pointer_type;

    /**
     * Creates a new even timing object and returns a shared pointer
     * to it.
     * @param duration the duration of the passage, in beats per minute.
     * @param bps the beat per minute throughout the passage.
     */
    static pointer_type
    create(units::beat duration, units::bps bps)
    {
        return pointer_type(new constant(duration, bps));
    }

    units::bps bps(units::beat /* beat */) const { return bps_; }
    units::beat time_to_beat(units::time time) const
    {
        return units::beat{bps_.value * time.value};
    }
    units::time beat_to_time(units::beat beat) const
    {
        return units::time{beat.value / bps_.value};
    }
    units::time full_time() const { return full_time_; }

    virtual ~constant() {}
};

/** Linear speedup/slowdown */
class linear : public timing {
    typedef std::shared_ptr<const linear> pointer_type;

    units::bps start_bps;
    units::bps end_bps;
    units::time full_time_;

    linear(units::beat duration, units::bps sbps, units::bps ebps)
        : timing{duration}
        , start_bps{sbps}
        , end_bps{ebps}
        , full_time_{
            (duration / units::interpolate(start_bps, end_bps, 0.5))*2}
    {}

public:
    static pointer_type
    create(units::beat duration, units::bps start_bps, units::bps end_bps)
    {
        return pointer_type(new linear(duration, start_bps, end_bps));
    }

    /**
     * Returns the current bps.
     */
    units::bps bps(units::beat beat) const
    {
        auto path = beat_to_time(beat)/full_time();
        return units::interpolate(start_bps, end_bps, path);
    }

    /**
     * The number of beats passed is the area of the trapese in the picture.
     * <pre>
     *     c = a(1-t) + bt
     *    /|
     * a / |
     *  |__|
     *  0  t
     * </pre>
     */
    units::beat time_to_beat(units::time time) const
    {
        auto T = full_time();
        auto frac = time/T;
        auto c = units::interpolate(start_bps, end_bps, frac);
        auto r = units::interpolate(start_bps, c, 0.5) * time;
        return r;
    }

    /**
     * Convert beat to time.
     *
     * We start with the inverse equation - converting time to beat.
     * So, d == t(a + (1-t/T)a + (t/T)b)/2
     * We take this equation and get t out:
     * d == a t + 1/(2T) * t^2(b - a)
     * We then extract t:
     * t = (aT - sqrt(2dT(b-a)+a^2 T^2))/(a-b)
     *
     */
    units::time beat_to_time(units::beat beat) const
    {
        auto s = start_bps.value;
        auto e = end_bps.value;
        auto T = full_time().value;
        auto b = beat.value;

        auto alpha = (e-s)/T;
        auto beta  = s*2;
        auto gamma = b*(-2);

        double t1, t2;
        std::tie(t1, t2) = math::quadratic_roots(alpha, beta, gamma);

        return units::time{(t1 >= 0)?t1 : t2};
    }

    /**
     * Returns the full time of the period.
     * <pre>
     * B ... length of period in beats
     * a ... start bps
     * b ... end bps
     * T ... full time
     *
     * In time T we must reach B full beats.
     * That means that the area of the trapese
     *     b
     *    /|
     * a / |
     *  |__|
     *  0  T
     * must be equal to B. The area of this trapese is (a + b)T/2.
     * This means 
     *   B = (a+b)/2 T
     * or alternatively
     *   T = 2 B / (a + b)
     * </pre>
     */
    units::time full_time() const { return full_time_; }

    virtual ~linear() {}

};

} /* end namespace timing */
} /* end namespace notation */
} /* end namespace sgr */

#endif
