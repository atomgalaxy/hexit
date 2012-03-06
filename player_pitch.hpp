#ifndef SGR_PLAYER_PITCH_HPP
#define SGR_PLAYER_PITCH_HPP
/**
 * @file player_pitch.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */

#include "notation_pitch.hpp"
#include "player_timing.hpp"
#include "math.hpp"
#include <memory>
#include <sstream>
#include <string>

namespace sgr {
namespace player {
namespace pitch {

struct pitch {
    typedef std::shared_ptr<pitch> pointer_type;
    virtual units::tone get_pitch(
            const timing::period& bounds, const timing::time& now) = 0;
    virtual std::string str() = 0;
    virtual ~pitch() {}
};

struct constant : public pitch {
    notation::pitch::constant data;

    constant(const decltype(data)& data)
        : data(data) {}

    virtual units::tone get_pitch(
            const timing::period& /* bounds */, const timing::time& /* now */)
    {
        return data.pitch;
    }

    std::string str() {
        std::stringstream ss;
        ss << "Constant pitch at " << data.pitch.str();
        return ss.str();
    }

    virtual ~constant() {}
};

struct linear_slide : public pitch {
    notation::pitch::linear_slide data;

    linear_slide(const decltype(data)& data)
        : data(data) {}

    virtual units::tone get_pitch(
            const timing::period& bounds, const timing::time& now)
    {
        auto path = bounds.beat_fraction(now);
        return units::tone{
            math::linear_interpolate(
                   data.start_pitch.value, data.end_pitch.value, path)};

    }

    std::string str() {
        std::stringstream ss;
        ss << "Linear pitch from " << data.start_pitch.str()
            << " to " << data.end_pitch.str();
        return ss.str();
    }

    virtual ~linear_slide() {}
};

namespace impl_detail {
    struct factory_visitor : public notation::pitch::pitch_visitor
    {
        pitch::pointer_type obj;
        factory_visitor()
            : obj(nullptr)
        {}

        void visit(const notation::pitch::constant& env) {
            obj = pitch::pointer_type(new constant(env));
        }
        void visit(const notation::pitch::linear_slide& env)
        {
            obj = pitch::pointer_type(new linear_slide(env));
        }


        decltype(obj)
        getobj()
        {
            return obj;
        }

    };
}

pitch::pointer_type
factory(const notation::pitch::pitch& instr)
{
    impl_detail::factory_visitor v;
    instr.accept(v);
    return v.getobj();
}

}/* end namespace pitch */
}/* end namespace player */
}/* end namespace sgr */



#endif
