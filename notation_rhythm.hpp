/**
 * @file notation_rhythm.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

namespace sgr {
namespace notation {
namespace rhythm {


/**
 * Holds a passage of hits.
 */
struct bar {
    bar() : hits_(), length_(0) {}
    bar& operator<<(const hit& h)
    {
        if (h.start + h.duration > length_) {
            length_ = h.start + h.duration;
        }
        hits_.push_back(h);
        return *this;
    }

    double length() const { return length_; }
    const std::vector<hit>& hits() const { return hits_; }
private:
    std::vector<hit> hits_;
    double length_;
};


} /* end namespace timing. */
} /* end namespace rhythm */
} /* end namespace notation */
} /* end namespace sgr */
