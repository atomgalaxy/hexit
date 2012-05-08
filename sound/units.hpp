#ifndef SGR_UNITS_HPP
#define SGR_UNITS_HPP
/**
 * @file units.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-09
 */

#include <sstream>
#include <string>
#include <cmath>
#include <vector>

#define GENERATE_OPERATOR2(Operator, ResultType, FirstType, SecondType) \
    inline constexpr \
    ResultType operator Operator(const FirstType& a1, const SecondType& a2) { \
        return ResultType{a1.value Operator a2.value}; \
    }

#define GENERATE_SCALAR_MULTIPLICATION(ArgType) \
    inline constexpr \
    ArgType operator *(const ArgType& a1, scalar t) { \
        return ArgType{a1.value * t}; \
    } \
    inline constexpr \
    ArgType operator *(scalar t, const ArgType& a1) { \
        return ArgType{a1.value * t}; \
    } \
    inline \
    ArgType& operator *=(ArgType& a1, scalar t) { \
        a1.value *= t; \
        return a1; \
    } \
    inline constexpr \
    ArgType operator /(const ArgType& a1, scalar t) { \
        return ArgType{a1.value / t}; \
    } \
    inline \
    ArgType& operator /=(ArgType& a1, scalar t) { \
        a1.value /= t; \
        return a1; \
    }

#define GENERATE_SCALAR_INVERSION(ReturnType, ArgType) \
    inline constexpr \
    ReturnType operator /(scalar t, const ArgType& a1) { \
        return ReturnType{a1.value / t}; \
    }

#define GENERATE_OPERATOR1(Operator, ResultType, ArgType) \
    inline constexpr \
    ResultType operator Operator(const ArgType& a) {\
        return ResultType{Operator(a.value)}; \
    }
#define GENERATE_REFOPERATOR2(Operator, FirstType, SecondType) \
    inline \
    FirstType& operator Operator(FirstType& a1, const SecondType& a2) { \
        a1.value Operator a2.value; \
        return a1; \
    }
#define GENERATE_INTERPOLATE(ArgType) \
    inline constexpr \
    ArgType interpolate(const ArgType& a1, const ArgType& a2, scalar t) {\
        return ArgType{a1.value * (1-t) + a2.value * t}; \
    }

#define GENERATE_COMPARABLE(ArgType) \
    GENERATE_OPERATOR2(<, bool, ArgType, ArgType) \
    GENERATE_OPERATOR2(<=, bool, ArgType, ArgType) \
    GENERATE_OPERATOR2(==, bool, ArgType, ArgType) \
    GENERATE_OPERATOR2(>=, bool, ArgType, ArgType) \
    GENERATE_OPERATOR2(>, bool, ArgType, ArgType)

#define GENERATE_ABELIAN_NO_INTERP(ArgType) \
    GENERATE_OPERATOR2(+, ArgType, ArgType, ArgType) \
    GENERATE_OPERATOR2(-, ArgType, ArgType, ArgType) \
    GENERATE_OPERATOR1(-, ArgType, ArgType) \
    GENERATE_REFOPERATOR2(+=, ArgType, ArgType) \
    GENERATE_REFOPERATOR2(-=, ArgType, ArgType)

#define GENERATE_ABELIAN(ArgType) \
    GENERATE_ABELIAN_NO_INTERP(ArgType) \
    GENERATE_INTERPOLATE(ArgType)

#define GENERATE_SUBSTRACTION(FirstType, SecondType) \
    GENERATE_OPERATOR2(-, FirstType, FirstType, SecondType) \
    GENERATE_REFOPERATOR2(-=, FirstType, SecondType)

#define GENERATE_ADDITION(FirstType, SecondType) \
    GENERATE_OPERATOR2(+, FirstType, FirstType, SecondType) \
    GENERATE_OPERATOR2(+, FirstType, SecondType, FirstType) \
    GENERATE_REFOPERATOR2(+=, FirstType, SecondType) \
    GENERATE_SUBSTRACTION(FirstType, SecondType)

#define GENERATE_MULTIPLICATION(ResultType, FirstType, SecondType) \
    GENERATE_OPERATOR2(*, ResultType, FirstType, SecondType) \
    GENERATE_OPERATOR2(*, ResultType, SecondType, FirstType)

#define GENERATE_DIVISION(ResultType, FirstType, SecondType) \
    GENERATE_OPERATOR2(/, ResultType, FirstType, SecondType)

#if __GNUC__ > 3 && __GNUC_MINOR__ > 6
# define GENERATE_LITERAL_INT_OPERATOR(Type, Suffix) \
    namespace literals { \
        Type operator "" Suffix(unsigned long long v) { \
            return Type{int(v)};\
        } \
    }

# define GENERATE_LITERAL_FLOAT_OPERATOR(Type, Suffix) \
    namespace literals { \
        Type operator "" Suffix(long double v) { \
            return Type{double(v)};\
        } \
    }
#else
# define GENERATE_LITERAL_INT_OPERATOR(Type, Suffix)
# define GENERATE_LITERAL_FLOAT_OPERATOR(Type, Suffix)
#endif


namespace sgr {
namespace units {

typedef double scalar;

static const scalar TAU = 2 * M_PI;
static const scalar TWELFTH_ROOT_OF_2 = 1.05946309435929;

/**
 * Represents the offset of a tone within a scale. For instance, in a major
 * scale, 2 means a major third, but in a minor scale, 2 means a minor third.
 */
struct scale_offset {
    int value;
    explicit constexpr scale_offset(decltype(value) value) : value{value} {}
};
GENERATE_ABELIAN_NO_INTERP(scale_offset)
GENERATE_COMPARABLE(scale_offset)
GENERATE_LITERAL_INT_OPERATOR(scale_offset, _in_scale)

/** Modifies tones. */
struct interval {
    scalar value;

    explicit constexpr interval(decltype(value) value) : value{value} {}
    constexpr interval(int octaves, decltype(value) value) : value{12 * octaves + value} {}
    constexpr interval(int octaves, interval value) : value{12 * octaves + value.value} {}
};
GENERATE_ABELIAN(interval)
GENERATE_COMPARABLE(interval)
GENERATE_LITERAL_FLOAT_OPERATOR(interval, _int)

/** Offset from A440, in semitones. */
struct tone {
    scalar value;

    explicit constexpr tone(decltype(value) value) : value{value} {}

    std::string str() {
        std::stringstream ss;
        ss << value << "{";
        int to_name = int(round(value))%12;
        if (to_name < 0) { to_name += 12; }
        switch (to_name) {
            case 0: ss << "A"; break;
            case 1: ss << "A# (B)"; break;
            case 2: ss << "H"; break;
            case 3: ss << "C"; break;
            case 4: ss << "C#"; break;
            case 5: ss << "D"; break;
            case 6: ss << "D#"; break;
            case 7: ss << "E"; break;
            case 8: ss << "F"; break;
            case 9: ss << "F#"; break;
            case 10: ss << "G"; break;
            case 11: ss << "G#"; break;
        }
        ss << "}";
        return ss.str();
    }
};
GENERATE_INTERPOLATE(tone)
GENERATE_COMPARABLE(tone)
GENERATE_ADDITION(tone, interval)
GENERATE_LITERAL_FLOAT_OPERATOR(tone, _tone)

/** In seconds. */
struct time {
    scalar value;
    explicit constexpr time(decltype(value) value) : value{value} {}
};
GENERATE_ABELIAN(time)
GENERATE_COMPARABLE(time)
GENERATE_LITERAL_FLOAT_OPERATOR(time, _s)

/** In Hz. */
struct frequency {
    scalar value;
    explicit constexpr frequency(decltype(value) value) : value{value} {}

    /**
     * Returns the frequency of the given tone.
     * @param tone the offset, in semitones, from a440. Negative values
     * are of course accepted.
     * @return the frequency, in Hz
     */
    frequency(tone t) : value{pow(TWELFTH_ROOT_OF_2, t.value) * 440} {}
};
GENERATE_INTERPOLATE(frequency)
GENERATE_COMPARABLE(frequency)
GENERATE_LITERAL_FLOAT_OPERATOR(frequency, _Hz)

/** In beats per second. */
struct bps {
    scalar value;
    explicit constexpr bps(decltype(value) value) : value{value} {}
};
GENERATE_INTERPOLATE(bps)
GENERATE_COMPARABLE(bps)
GENERATE_LITERAL_FLOAT_OPERATOR(bps, _bps)

struct beat {
    scalar value;
    explicit constexpr beat(decltype(value) value) : value{value} {}
};
GENERATE_ABELIAN(beat)
GENERATE_COMPARABLE(beat)
GENERATE_SCALAR_MULTIPLICATION(beat)
GENERATE_LITERAL_FLOAT_OPERATOR(beat, _beats)

GENERATE_MULTIPLICATION(beat, time, bps)
GENERATE_SCALAR_MULTIPLICATION(time)
GENERATE_SCALAR_INVERSION(frequency, time)
GENERATE_DIVISION(time, beat, bps)
GENERATE_DIVISION(scalar, time, time)
GENERATE_DIVISION(scalar, beat, beat)
GENERATE_DIVISION(scalar, bps, bps)
GENERATE_MULTIPLICATION(scalar, frequency, time)

/// a chord is a series of tone-offsets relative to a scale.
/// for instance, 0, 2, 4 (1, 3, 5) is the usual chord, that, when played in a
/// major scale, gives a major chord.
typedef std::vector<units::scale_offset> chord;
typedef std::vector<units::interval> intervals_type;
typedef std::vector<units::tone> tones_type;

tones_type
operator+(const intervals_type& ints, units::tone tone)
{
    tones_type out;
    out.reserve(ints.size());
    for (auto i : ints) {
        out.emplace_back(tone + i);
    }
    return out;
}

tones_type
operator+(units::tone tone, const intervals_type& ints)
{
    return ints + tone;
}


}
}


// UNDEF THE MACROS
#undef GENERATE_ABELIAN
#undef GENERATE_ABELIAN_NO_INTERP
#undef GENERATE_ADDITION
#undef GENERATE_COMPARABLE
#undef GENERATE_DIVISION
#undef GENERATE_INTERPOLATE
#undef GENERATE_MULTIPLICATION
#undef GENERATE_OPERATOR1
#undef GENERATE_OPERATOR2
#undef GENERATE_REFOPERATOR2
#undef GENERATE_SCALAR_INVERSION
#undef GENERATE_SCALAR_MULTIPLICATION
#undef GENERATE_SUBSTRACTION
#undef GENERATE_LITERAL_INT_OPERATOR
#undef GENERATE_LITERAL_FLOAT_OPERATOR

#endif
