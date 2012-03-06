#ifndef SGR_NOTATION_PITCH
#define SGR_NOTATION_PITCH
/**
 * @file notation_pitch.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

#include <memory>

#include "units.hpp"

namespace sgr {
namespace notation {
namespace pitch {

// forward declarations
struct constant;
struct linear_slide;

// visitor
struct pitch_visitor {
    virtual void visit(const linear_slide& p) = 0;
    virtual void visit(const constant& p) = 0;
    virtual ~pitch_visitor() {}
};

/// Abstract base class for pitch envelopes.
struct pitch {
    typedef std::shared_ptr<const pitch> pointer_type;
    /// visitor interface
    virtual void accept(pitch_visitor& vis) const = 0;
    virtual ~pitch() {}
};

// ###############
// PITCH ENVELOPES
// ###############

/**
 * Implements a constant pitch envelope.
 */
struct constant : public pitch {
    typedef std::shared_ptr<const constant> pointer_type;

    /// Pitch, in halftones from a440.
    units::tone pitch;


    /// create new pitch envelopes through this function.
    /// @param pitch the pitch of the note, in halftone offsets from a440.
    /// @return a new constant pitch object.
    static pointer_type
    create(decltype(pitch) pitch) { return pointer_type(new constant(pitch)); }

    /// visitor implementation
    void accept(pitch_visitor& vis) const { vis.visit(*this); }

    /// Virtual destructor.
    virtual ~constant() {}


    private:
    constant(decltype(pitch) pitch) : pitch(pitch) {}
};

/**
 * Implements a linear_slide pitch envelope.
 */
struct linear_slide : public pitch {
    typedef std::shared_ptr<const linear_slide> pointer_type;
    units::tone start_pitch;
    units::tone end_pitch;

    /// create new pitch envelopes through this function.
    /// @param pitch the pitch of the note, in halftone offsets from a440.
    /// @return a new linear_slide pitch object.
    static pointer_type
    create(decltype(start_pitch) start_pitch, decltype(end_pitch) end_pitch) {
        return pointer_type(new linear_slide(start_pitch, end_pitch));
    }

    /// visitor implementation
    void accept(pitch_visitor& vis) const { vis.visit(*this); }

    /// Virtual destructor.
    virtual ~linear_slide() {}


    private:
    linear_slide(decltype(start_pitch) start_pitch, decltype(end_pitch) end_pitch)
        : start_pitch{start_pitch}, end_pitch{end_pitch}
    {}
};


} /* end namespace pitch */
} /* end namespace notation */
} /* end namespace sgr */

#endif
