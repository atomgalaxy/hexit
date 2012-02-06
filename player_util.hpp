#ifndef SGR_PLAYER_UTIL_HPP
#define SGR_PLAYER_UTIL_HPP
/**
 * @file player_util.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */

#include <cmath>

namespace sgr {
    namespace player {
        namespace util {
            static const double TAU = 2 * M_PI;
            static const double TWELFTH_ROOT_OF_2 = 1.05946309435929;

            /**
             * Returns the frequency of the given tone.
             * @param tone the offset, in semitones, from a440. Negative values
             * are of course accepted.
             * @return the frequency, in Hz
             */
            double tone_to_freq(double tone)
            {
                double shift = pow(TWELFTH_ROOT_OF_2, tone);
                return 440 * shift;
            }
        } /* end namespace util */
    }/* end namespace player */
}/* end namespace sgr */

#endif
