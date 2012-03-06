#ifndef SGR_SCALARS_HPP
#define SGR_SCALARS_HPP
/**
 * @file scalars.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */
#include <sstream>
#include <string>

namespace sgr {
namespace scalars {
    /** Holds volumes. Can be used as a volume accumulator etc. */
    struct volume {
        double left;
        double right;
        double full;

        volume(decltype(left) left, decltype(right) right,
                decltype(full) full = 1)
            : left(left)
            , right(right)
            , full(full)
        {}

        volume operator+(const volume& o) const
        {
            return volume{left + o.left, right+o.right, full + o.full};
        }

        volume& operator+=(const volume& o) {
            left += o.left;
            right += o.right;
            full += o.full;
            return *this;
        }

        volume operator*(const volume& o) const {
            auto norm = full * o.full;
            auto l = left * o.left / norm;
            auto r = right * o.right / norm;
            return volume{l,r,1};
        }

        volume& operator*=(const volume& o) {
            auto norm = full * o.full;
            left *= o.left / norm;
            right *= o.right / norm;
            full = 1;
            return *this;
        }

        volume operator*(const double o) const {
            return volume{left * o, right * o, full};
        }

        volume& operator*=(const double o) {
            left *= o;
            right *= o;
            return *this;
        }

        volume operator/(const double o) const {
            return volume{left / o, right / o, full};
        }

        volume& operator/=(const double o) {
            left /= o;
            right /= o;
            return *this;
        }

        std::string str() {
            std::stringstream ss;
            ss << "{" << left << ", " << right << "}/" << full;
            return ss.str();
        }
    };

    /** 
     * A 2-vector, holds a tuple of left and right amplitudes.
     */
    struct sample {
        double left;
        double right;

        sample(decltype(left) left = 0, decltype(right) right = 0)
            : left(left)
             , right(right)
        {}
        sample operator*(const volume& v) const {
            return sample{
                    left * v.left/v.full,
                    right * v.right/v.full
                   };
        }
        sample& operator*=(const volume& v) {
            left  *= v.left/v.full;
            right *= v.right/v.full;
            return *this;
        }
        sample operator+(const sample& o) const {
            return sample{left + o.left, right + o.right};
        }
        sample& operator+=(const sample& o) {
            left += o.left;
            right += o.right;
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
    }; /* end struct sample */
}
}
#endif
