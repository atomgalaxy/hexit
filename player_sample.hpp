#ifndef SGR_PLAYER_SAMPLE_HPP
#define SGR_PLAYER_SAMPLE_HPP
/**
 * @file player_sample.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */

namespace sgr {
    namespace player {
        /** A 2-vector, holds a tuple of left and right amplitudes.
         *  Also used to hold volumes.
         */
        struct sample
        {
            double left;
            double right;


            sample(double left = 0, double right = 0)
                : left(left)
                  , right(right)
            {}
            sample& operator*=(const sample& other) {
                left *= other.left;
                right *= other.right;
                return *this;
            }
            sample operator*(const sample& other) const {
                return sample(left * other.left, right * other.right);
            }
            sample operator*(double t) const {
                return sample(left * t, right * t);
            }
            sample operator+(const sample& other) const {
                return sample(left + other.left, right + other.right);
            }
            sample& operator+=(const sample& other) {
                left += other.left;
                right += other.right;
                return *this;
            }
            sample& operator*=(double t) {
                left *= t;
                right *= t;
                return *this;
            }
            /** Forces the sample into the correct values from minus one to one. */
            sample& clip() {
                if (left < -1) {
                    left = -1;
                } else if (left > 1) {
                    left = 1;
                }
                if (right < -1) {
                    right = -1;
                } else if (right > 1) {
                    right = 1;
                }
                return *this;
            }
            /** Forces the sample into the correct values from zero to one. */
            sample& clip_01() {
                if (left < 0) {
                    left = 0;
                } else if (left > 1) {
                    left = 1;
                }
                if (right < 0) {
                    right = 0;
                } else if (right > 1) {
                    right = 1;
                }
                return *this;
            }


        }; /* end struct sample */
    }
}
#endif
