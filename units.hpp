#ifndef SGR_UNITS_HPP
#define SGR_UNITS_HPP
/**
 * @file units.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-09
 */

namespace sgr {
namespace units {

typedef double scalar;
static const scalar TAU = 2 * M_PI;
static const scalar TWELFTH_ROOT_OF_2 = 1.05946309435929;
/** Represents the offset of a tone within a scale. For instance, in a major
 * scale, 2 means a major third, but in a minor scale, 2 means a minor third.
 */
struct scale_offset {
    int value;
    explicit constexpr scale_offset(decltype(value) value) : value{value} {}

    constexpr scale_offset operator+(const scale_offset& o) const {
        return scale_offset{value + o.value};
    }
    scale_offset& operator+=(const scale_offset& o) {
        value += o.value;
        return *this;
    }
    constexpr scale_offset operator-(const scale_offset& o) const {
        return scale_offset{value - o.value};
    }
    scale_offset& operator-=(const scale_offset& o) {
        value -= o.value;
        return *this;
    }
};

/** Modifies tones. */
struct interval {
    double value;

    explicit constexpr interval(decltype(value) value) : value{value} {}
    constexpr interval(int octaves, decltype(value) value) : value{12 * octaves + value} {}
    constexpr interval(int octaves, interval value) : value{12 * octaves + value.value} {}

    constexpr interval operator+(const interval& o) const {
        return interval{value + o.value};
    }
    interval& operator+=(const interval& o) {
        value += o.value;
        return *this;
    }
    constexpr interval operator-(const interval& o) const {
        return interval(value - o.value);
    }
    interval& operator-=(const interval& o) {
        value -= o.value;
        return *this;
    }
};

/** Offset from A440, in semitones. */
struct tone {
    double value;
    explicit constexpr tone(decltype(value) value) : value{value} {}
    constexpr tone operator+(const interval& o) const {
        return tone{value + o.value};
    }
    tone& operator+=(const interval& o) {
        value += o.value;
        return *this;
    }
};

/** In seconds. */
struct time {
    double value;
    explicit constexpr time(decltype(value) value) : value{value} {}

    constexpr time operator*(const scalar sc) const {
        return time{value * sc};
    }

    time& operator*=(const scalar sc) {
        value *= sc;
        return *this;
    }

    constexpr time operator+(const time& other) const {
        return time{value + other.value};
    }

    constexpr time operator-(const time& other) const {
        return time{value - other.value};
    }

    time& operator+=(const time& other) {
        value += other.value;
        return *this;
    }

    constexpr scalar operator/(const time& other) const {
        return value/other.value;
    }

    bool operator<(const time& other) const {
        return value<other.value;
    }

    bool operator>(const time& other) const {
        return value>other.value;
    }

    bool operator<=(const time& other) const {
        return value<=other.value;
    }

    bool operator>=(const time& other) const {
        return value>=other.value;
    }
};

/** In Hz. */
struct frequency {
    double value;
    explicit constexpr frequency(decltype(value) value) : value{value} {}

    /**
     * Returns the frequency of the given tone.
     * @param tone the offset, in semitones, from a440. Negative values
     * are of course accepted.
     * @return the frequency, in Hz
     */
    frequency(tone t) : value{pow(TWELFTH_ROOT_OF_2, t.value) * 440} {}

    frequency operator*(const scalar sc) const {
        return frequency{value * sc};
    }

    frequency& operator*=(const scalar& sc) {
        value *= sc;
        return *this;
    }

    scalar operator*(const time& t) const {
        return scalar{t.value * value};
    }
};

struct beat;
/** In beats per second. */
struct bps {
    double value;
    explicit constexpr bps(decltype(value) value) : value{value} {}

    bps operator+(const bps other) const {
        return bps{value + other.value};
    }

    bps operator-(const bps other) const {
        return bps{value - other.value};
    }

    bps operator*(const scalar sc) const {
        return bps{value * sc};
    }
    bps operator/(const scalar sc) const {
        return bps{value * sc};
    }

    bps& operator*=(const scalar& sc) {
        value *= sc;
        return *this;
    }

    beat operator*(const time& t) const;
};

struct beat {
    double value;
    explicit beat(decltype(value) value) : value{value} {}

    beat operator*(const scalar sc) const {
        return beat{value * sc};
    }

    beat& operator*=(const scalar sc) {
        value *= sc;
        return *this;
    }

    beat operator+(const beat& other) const {
        return beat{value + other.value};
    }

    beat operator-(const beat& other) const {
        return beat{value - other.value};
    }

    beat operator-() const {
        return beat{-value};
    }

    beat& operator+=(const beat& other) {
        value += other.value;
        return *this;
    }

    beat& operator-=(const beat& other) {
        value -= other.value;
        return *this;
    }

    scalar operator/(const beat& other) const {
        return value/other.value;
    }

    bool operator<(const beat& other) const {
        return value<other.value;
    }

    bool operator>(const beat& other) const {
        return value>other.value;
    }

    bool operator<=(const beat& other) const {
        return value<=other.value;
    }

    bool operator>=(const beat& other) const {
        return value>=other.value;
    }

    units::time operator/(const bps& other) const {
        return time{value/other.value};
    }

};

beat bps::operator*(const time& t) const
{
    return beat{value * t.value};
}


}
}
#endif
