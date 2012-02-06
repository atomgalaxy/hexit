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

namespace sgr {
    namespace math {
        /**
         * A % B / B, for doubles. So, it tells us the fraction of B that
         * remains after division. Faster than mod.
         */
        double pmod(double a, double b)
        {
            return (a/b - floor(a/b));
        }

        /**
         * A % B, but for doubles.
         */
        double mod(double a, double b)
        {
            return (a/b - floor(a/b)) * b;
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
