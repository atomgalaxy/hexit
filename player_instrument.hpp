#ifndef SGR_PLAYER_INSTRUMENT
#define SGR_PLAYER_INSTRUMENT
/**
 * @file player_instrument.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-06
 */

#include "notation_instrument.hpp"
#include "player_timing.hpp"
#include "player_util.hpp"
#include "scalars.hpp"
#include "units.hpp"

#include <memory>
#include <cmath> //for sin

namespace sgr {
namespace player {
namespace instrument {

/** Instrument ABS. Specifies the minimal interface for instruments. */
struct instrument {
    typedef std::shared_ptr<instrument> pointer_type;
    /**
     * Returns the sample for the current time.
     * @param bounds when the tone starts and ends.
     * @param now The current time is determined by the now parameter, which
     * MUST have dt and dbeat set to a nonzero value.
     * @param volume the volume for the left and right channels
     * @param pitch the current pitch.
     * @return the sample for the current time.
     */
    virtual scalars::sample get_sample(
            const timing::period& bounds,
            const timing::time& now,
            const scalars::volume& volume,
            units::tone pitch) = 0;
    virtual ~instrument() {}
};

/**
 * Plays a sine.
 */
struct sinewave : public instrument
{
    sinewave(const notation::instrument::sinewave& data)
        : data(data), wheel(0) {}

    virtual scalars::sample get_sample(
            const timing::period& /* bounds */,
            const timing::time& now,
            const scalars::volume& volume,
            const units::tone pitch)
    {
        wheel += units::frequency{pitch} * now.dt;
        double x = wheel * units::TAU;
        double sx = sin(x);

        return scalars::sample{sx, sx} * volume;
    }

    virtual ~sinewave() {}

    private:
    notation::instrument::sinewave data;
    double wheel;
};

/**
 * Produces a wave like
 * <pre>
 * /\/\/\/\/\/\/\/\/\/\/\/
 * </pre>
 */
struct sawwave : public instrument
{
    sawwave(const notation::instrument::sawwave& data)
        : data(data), wheel(0) {}

    virtual scalars::sample get_sample(
            const timing::period& /* bounds */,
            const timing::time& now,
            const scalars::volume& volume,
            const units::tone pitch)
    {
        auto freq = units::frequency{pitch};
        wheel += freq * now.dt;
        if (wheel > 1) {
            wheel -= floor(wheel);
        }

        double a = 0;
        if (wheel < 0.5) { // up slope
            a = -1 + 4*wheel;
        } else {
            a = 1 - 4*(wheel - 0.5);
        }

        return scalars::sample{a,a} * volume;
    }

    virtual ~sawwave() {}

    private:
    notation::instrument::sawwave data;
    double wheel;
};

/**
 * Produces a wave like
 * <pre>
 * ___     ___     ___
 *    |___|   |___|   |___
 * </pre>
 */
struct squarewave : public instrument
{
    squarewave(const notation::instrument::squarewave& data)
        : data(data), wheel(0) {}

    virtual scalars::sample get_sample(
            const timing::period& /* bounds */,
            const timing::time& now,
            const scalars::volume& volume,
            units::tone pitch)
    {
        wheel += units::frequency{pitch} * now.dt;
        if (wheel > 1) {
            wheel -= floor(wheel);
        }

        double a = 0;
        if (wheel < 0.5) { // bottom of waveform
            a = -1;
        } else {          // top of waveform
            a = 1;
        }
        return scalars::sample{a,a} * volume;
    }

    virtual ~squarewave() {}

    private:
    notation::instrument::squarewave data;
    double wheel;
};

namespace impl_detail {
    struct factory_visitor : public notation::instrument::instrument_visitor
    {
        instrument::pointer_type obj;
        factory_visitor()
            : obj(nullptr)
        {}

        void visit(const notation::instrument::sinewave& env)
        {
            obj = instrument::pointer_type(new sinewave(env));
        }
        void visit(const notation::instrument::sawwave& env)
        {
            obj = instrument::pointer_type(new sawwave(env));
        }
        void visit(const notation::instrument::squarewave& env)
        {
            obj = instrument::pointer_type(new squarewave(env));
        }


        decltype(obj)
        getobj()
        {
            return obj;
        }

    };
}

instrument::pointer_type
factory(const notation::instrument::instrument& instr)
{
    impl_detail::factory_visitor v;
    instr.accept(v);
    return v.getobj();
}

} /* end namespace instrument */
}/* end namespace player */
}/* end namespace sgr */

#endif
