#ifndef SGR_NOTATION_VOLUME_HPP
#define SGR_NOTATION_VOLUME_HPP
/**
 * @file notation_volume.hpp
 * Implements the sgr::notation::volume namespace.
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

#include <memory>

namespace sgr {
namespace notation {
namespace volume {


/* forward declarations */
struct simple;

/* interfaces */
struct volume_visitor {
    virtual void visit(const simple& s) = 0;
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
struct simple : public volume {
private:
    simple(double vol_left, double vol_right)
        : left(vol_left)
        , right(vol_right)
    {}
public:
    /**
     * This is how you create these objects - since they must always be stored
     * in a shared pointer.
     * @param vol_left the left volume. In the range [0,1].
     * @param vol_right the right volume. In the range [0,1].
     * @return a std::shared_ptr to the created object. The object is immutable.
     */
    static std::shared_ptr<const simple>
    create(double vol_left = 0.7, double vol_right=0.7)
    {
        return std::shared_ptr<const simple>(new simple(vol_left, vol_right));
    }

    /** left channel volume */
    const double left;
    /** right channel volume */
    const double right;

    /// visitor interface implementation.
    virtual void accept(volume_visitor& v) const { v.visit(*this); }
    /// destructor has to be virtual
    virtual ~simple() {}
};


} /* end namespace volume */
} /* end namespace notation */
} /* end namespace sgr */

#endif
