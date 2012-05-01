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
         * Returns 
         * 1  if a > 0
         * 0  if a = 0
         * -1 if a < 0
         */
        double sign(double a) {
            return (a > 0)?1 : ((a<0)?-1 : 0);
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

        auto quadratic_roots(double a, double b, double c)
            -> decltype(std::make_tuple(a, b))
        {
            using std::make_tuple;
            auto q = -0.5 * (b + sign(b)*sqrt(b*b - 4 * a * c));
            auto x1 = q/a;
            auto x2 = c/q;
            return std::make_tuple(x1, x2);
        }
    } /* end namespace math */
}

#endif
