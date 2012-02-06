#ifndef SGR_PLAYER_VOLUME_HPP
#define SGR_PLAYER_VOLUME_HPP
/**
 * @file player_volume.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-02-05
 */

#include "player_sample.hpp"
#include "player_timing.hpp"
#include "notation_volume.hpp"
#include "math.hpp"
#include <memory>
#include <cmath> //for std::min

namespace sgr {
namespace player {
namespace volume {

    /** volume interface */
    struct volume {
        typedef std::shared_ptr<volume> pointer_type;
        constexpr static double GLOBAL_FALLOFF = 0.02;
        /** Gets the decrackle volume. This should be used by all
         * implementations of get_volume to avoid crackling on tone boundaries.
         * @return 1 normally, but raises and lowers volume linearly between 1
         * and zero close to boundary.
         */
        double decrackle_volume(
                const timing::period& bounds,
                const timing::time& now
                )
        {
            using std::min;
            // decrackle
            double start = bounds.start.t;
            double end   = bounds.end.t;
            double t     = now.t;
            return min(min(t - start, end - t) / GLOBAL_FALLOFF, 1.0);
        }
        virtual sample get_volume(
                const timing::period& bounds,
                const timing::time& now) = 0;

        virtual ~volume() {}
    };

    struct simple : public volume
    {
        simple( const notation::volume::simple& data )
            : data(data) {}

        virtual sample get_volume(
                const timing::period& bounds,
                const timing::time& now
                )
        {
            double decrackle = decrackle_volume(bounds, now);
            return sample(data.left * decrackle, data.right * decrackle);
        }

        virtual ~simple() {}

        notation::volume::simple data;
    };

    struct fade : public volume
    {
        fade( const notation::volume::fade& data ) : data(data) {}

        virtual sample get_volume(
                const timing::period& bounds,
                const timing::time& now
                )
        {
            double decrackle = decrackle_volume(bounds, now);
            double path = bounds.beat_fraction(now);
            double left = math::linear_interpolate(
                    data.start_left, data.end_left, path);
            double right = math::linear_interpolate(
                    data.start_right, data.end_right, path);
            return sample(left * decrackle, right * decrackle);
        }

        virtual ~fade() {}

        notation::volume::fade data;
    };

    namespace impl_detail {
        struct factory_visitor : public notation::volume::volume_visitor
        {
            typedef std::shared_ptr<volume> obj_ptr_t;

            factory_visitor() : obj(nullptr) {}

            void visit(const notation::volume::simple& env)
            {
                obj = obj_ptr_t(new simple(env));
            }
            void visit(const notation::volume::fade& env)
            {
                obj = obj_ptr_t(new fade(env));
            }

            obj_ptr_t getobj() { return obj; }

            obj_ptr_t obj;
        };
    }

    volume::pointer_type
    factory(const notation::volume::volume& vol)
    {
        impl_detail::factory_visitor v;
        vol.accept(v);
        return v.getobj();
    }


} /* end namespace volume */
} /* end namespace player */
} /* end namespace sgr */
#endif
