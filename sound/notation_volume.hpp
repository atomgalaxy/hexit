#ifndef SGR_NOTATION_VOLUME_HPP
#define SGR_NOTATION_VOLUME_HPP
/**
 * @file notation_volume.hpp
 * Implements the sgr::notation::volume namespace.
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

#include "scalars.hpp"

#include <memory>

namespace sgr {
namespace notation {
namespace volume {


/* forward declarations */
struct simple;
struct fade;

/* interfaces */
struct volume_visitor {
    virtual void visit(const simple& s) = 0;
    virtual void visit(const fade& s) = 0;
    virtual ~volume_visitor() {}
};

struct volume {
    /// volume pointer type
    typedef std::shared_ptr<const volume> pointer_type;

    virtual void accept(volume_visitor& vis) const = 0;
    virtual ~volume() {}
};

/* ################
 * volume volumes
 * ################ */

/**
 * Simple volume volume - does nothing but provides constant volume throughout
 * the tone.
 */
class simple : public volume {
public:
    scalars::volume volume;

private:
    simple(decltype(volume) volume) : volume{volume} {}

public:
    /**
     * This is how you create these objects - since they must always be stored
     * in a shared pointer.
     * @param vol_left the left volume. In the range [0,1].
     * @param vol_right the right volume. In the range [0,1].
     * @return a std::shared_ptr to the created object. The object is immutable.
     */
    static std::shared_ptr<const simple>
    create(scalars::volume volume_)
    {
        return std::shared_ptr<const simple>(new simple(volume_));
    }


    /// visitor interface implementation.
    virtual void accept(volume_visitor& v) const { v.visit(*this); }
    /// destructor has to be virtual
    virtual ~simple() {}
};

/**
 * Fade volume volume - varies the tone volume from start volume to end volume
 * linearly.
 */
struct fade : public volume {
    scalars::volume start_volume;
    scalars::volume end_volume;

private:
    fade(scalars::volume start_volume, scalars::volume end_volume)
        : start_volume(start_volume)
        , end_volume(end_volume)
    {}

public:

    /**
     * This is how you create these objects - since they must always be stored
     * in a shared pointer.
     * @param vol_left the left volume. In the range [0,1].
     * @param vol_right the right volume. In the range [0,1].
     * @return a std::shared_ptr to the created object. The object is immutable.
     */
    static std::shared_ptr<const fade>
    create(scalars::volume start_volume, scalars::volume end_volume)
    {
        return std::shared_ptr<const fade>(new fade(start_volume, end_volume));
    }


    /// visitor interface implementation.
    virtual void accept(volume_visitor& v) const { v.visit(*this); }
    /// destructor has to be virtual
    virtual ~fade() {}
};


} /* end namespace volume */
} /* end namespace notation */
} /* end namespace sgr */

#endif
