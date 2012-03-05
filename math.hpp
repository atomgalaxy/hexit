#ifndef SGR_MATH_HPP
#define SGR_MATH_HPP
/**
 * @file math.hpp
 *
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-30
 */

#include <cmath>
#include <tuple>

namespace sgr {
    namespace math {
        /**
         * Divides two numbers and returns a tuple of (division result,
         * remainder).
         * It holds that divisor * result + remainder = factor.
         * Signs: the sign of the remainder is the same as the sign of the
         * divisor. The sign of the result matches factor * divisor.
         */
        template <typename T>
        auto fmod(T factor, T divisor)
            -> decltype(std::make_tuple(factor, factor))
        {
            using std::make_tuple;
            auto f = floor(factor / divisor);
            auto r = std::fmod(factor, divisor);
            if (f > 0) {
                return make_tuple(f,r);
            } else {
                return make_tuple(f, r + factor);
            }
        }
        /**
         * Linear interpolation between a and b,
         * t within 0-1 (not checked).
         * @param a the first vector/number
         * @param b the second vector/number
         * @param t the parameter of the interpolation
         * @return (1-t)*a + t*b
         */
        template <typename T>
        T linear_interpolate(const T a, const T b, double t)
        {
            return a * (1-t) + b * t;
        }
    } /* end namespace math */
}

#endif
