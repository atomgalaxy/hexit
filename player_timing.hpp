#ifndef SGR_PLAYER_TIMING_HPP
#define SGR_PLAYER_TIMING_HPP

/**
 * @file player_timing.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */
#include "units.hpp"

namespace sgr {
    namespace player {
        namespace timing {
            /** 
             * Represents a single point in time, plus its precision.
             */
            struct time {
                /// The time, in seconds since start of song.
                units::time t;
                /// The number of the beat, since start of song.
                units::beat beat;
                /** The time difference between two consecutive time structs.
                 * More or less constant, but we can afford it, and this allows
                 * for more flexible timing. You should use this for resolution
                 * measurements etc., since beat will probably never be an
                 * integer, and neither will time.
                 *
                 * For definite times (like starts and ends of notes), this
                 * should be zero.
                 */
                units::time dt;
                /**
                 * Difference between two consecutive beat timings. You can use
                 * this to detect start of beat etc. like so:
                 * if (beat - floor(beat) < dbeat) {yeah, this beat has just
                 * started}
                 *
                 * For definite beats (like starts and ends of notes), this
                 * should be zero.
                 */
                units::beat dbeat;
                time(decltype(t) t, decltype(beat) beat,
                     decltype(dt) dt = units::time{0},
                     decltype(dbeat) dbeat = units::beat{0})
                    : t(t), beat(beat), dt(dt), dbeat(dbeat) {}
            };

            struct period {
                /** The start time of the note. */
                time start;
                /** The end time of the note. */
                time end;

                period(time start, time end)
                    : start(start), end(end) {}

                /**
                 * Given a timepoint, returns what fraction of beats has
                 * already passed.
                 * @return 0-1, how far we are through this timeperiod, in
                 * beats.
                 */
                double beat_fraction(const time& now) const
                {
                    return (now.beat - start.beat)/(end.beat - start.beat);
                }
                /**
                 * Given a timepoint, returns what fraction of time has already
                 * passed.
                 * @return 0-1, how far we are through this timeperiod, in
                 * time.
                 */
                double time_fraction(const time& now) const
                {
                    return (now.t - start.t)/(end.t - start.t);
                }
            };


        }/* end namespace timing */
    }/* end namespace player */
}/* end namespace sgr */

#endif
