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
        double pmod(double a, double b)
        {
            return (a/b - floor(a/b));
        }

        double mod(double a, double b)
        {
            return (a/b - floor(a/b)) * b;
        }
    } /* end namespace math */
}

#endif
