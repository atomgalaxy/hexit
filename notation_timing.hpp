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
#include <memory>

namespace sgr {
namespace notation {
namespace timing {

struct timing {
    typedef std::shared_ptr<const timing> pointer_type;
    timing(double duration) : duration(duration) {}
    double duration; ///< in beat
    /// Should return the current bps, in beat since the start of this timing
    /// sequence
    virtual double bps(double beat) const = 0;
    /**
     * Given a time point (with 0 being the beginning of this timing period),
     * calculates the number of beat that have passed since the beginning of
     * this time period.
     */
    virtual double time_to_beat(double time) const = 0;
    /**
     * Given a beat point, calculates the time that has passed in order to reach
     * that beat. Zero coresponds to beginning of this time period, in both
     * ways.
     */
    virtual double beat_to_time(double beat) const = 0;
    /**
     * Returns the full time of the timing sequence.
     */
    virtual double full_time() const = 0;
    /// virtual dtor
    virtual ~timing() {}
};

/** Constant bpm timing.  */
struct constant : public timing {
    typedef std::shared_ptr<const constant> pointer_type;

    /**
     * Creates a new even timing object and returns a shared pointer
     * to it.
     * @param bpt the beat per minute throughout the passage.
     */
    static pointer_type
    create(double duration, double bps)
    {
        return pointer_type(new constant(duration, bps));
    }

    double bps(double /* beat */) const { return bps_; }
    double time_to_beat(double time) const
    {
        return bps_ * time;
    }
    double beat_to_time(double beat) const
    {
        return beat / bps_;
    }
    double full_time() const { return full_time_; }

    virtual ~constant() {}

    private:
    constant(double duration, double bps)
        : timing(duration), bps_(bps), full_time_(duration / bps) {}
    double bps_;
    double full_time_;
};

/** Linear speedup/slowdown */
struct linear : public timing {
    typedef std::shared_ptr<const linear> pointer_type;

    static pointer_type
    create(double duration, double start_bps, double end_bps)
    {
        return pointer_type(new linear(duration, start_bps, end_bps));
    }

    double bps(double beat) const
    {
        double path = beat_to_time(beat)/full_time();
        return math::linear_interpolate(start_bps, end_bps, path);
    }
    /**
     * The number of beat passed is the area of the trapese in the picture.
     * <pre>
     *     a(1-t) + bt
     *    /|
     * a / |
     *  |__|
     *  0  t
     * </pre>
     * 
     */
    double time_to_beat(double time) const
    {
        double T = full_time();
        double frac = time/T;
        double b = math::linear_interpolate(start_bps, end_bps, frac);
        return (start_bps + b)/2 * time;
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
    double beat_to_time(double beat) const
    {
        double a = start_bps;
        double b = end_bps;
        double T = full_time();
        double d = beat;
        double aa = a*a;
        double TT = T*T;
        return (a*T - sqrt(2*d*T*(b-a) + aa * TT))/(a-b);
    }
    /**
     * Returns the full time of the period.
     * <pre>
     * B ... length of period in beat
     * a ... start bps
     * b ... end bps
     * T ... full time
     *
     * In time T we must reach B full beat.
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
    double full_time() const { return full_time_; }

    virtual ~linear() {}
    private:
    linear(double duration, double sbps, double ebps)
        : timing(duration)
        , start_bps(sbps)
        , end_bps(ebps)
        , full_time_(2 * duration / (start_bps + end_bps))
    {}

    double start_bps;
    double end_bps;
    double full_time_;
};

} /* end namespace timing */
} /* end namespace notation */
} /* end namespace sgr */

#endif
