#ifndef SGR_NOTATION_TIMING_HPP
#define SGR_NOTATION_TIMING_HPP
/**
 * @file notation_timing.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

#include <memory>

namespace sgr {
namespace notation {
namespace timing {

struct timing {
    typedef std::shared_ptr<const timing> pointer_type;
    timing(double duration) : duration(duration) {}
    size_t duration; ///< in beats
    /// Should return the current bps, in beats since the start of this timing
    /// sequence
    virtual double bps(double beats) = 0;
};

/** Constant bpm timing.  */
struct constant : public timing {
    typedef std::shared_ptr<const constant> pointer_type;

    /**
     * Creates a new even timing object and returns a shared pointer
     * to it.
     * @param bpt the beats per minute throughout the passage.
     */
    static pointer_type
    create(size_t duration, double bps)
    {
        return pointer_type(new constant(duration, bps));
    }

    double bps(double beats) { return bps_; }

    virtual ~constant() {}

    private:
    constant(size_t duration, double bps) : timing(duration), bps_(bps) {}
    double bps_;
};

/** Linear speedup/slowdown */
struct linear : public timing {
    typedef std::shared_ptr<const linear> pointer_type;

    static pointer_type
    create(double duration, double start_bps, double end_bps)
    {
        return pointer_type(new linear(duration, start_bps, end_bps));
    }

    double bps(double beats)
    {
        return (beats/duration) * (end_bps - start_bps) + start_bps;
    }

    virtual ~linear() {}
    private:
    linear(size_t duration, double sbps, double ebps)
        : timing(duration)
        , start_bps(sbps)
        , end_bps(ebps)
    {}

    double start_bps;
    double end_bps;
};

} /* end namespace timing */
} /* end namespace notation */
} /* end namespace sgr */

#endif
