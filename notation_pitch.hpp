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


    /// create new pitch envelopes through this function.
    /// @param pitch the pitch of the note, in halftone offsets from a440.
    /// @return a new constant pitch object.
    static pointer_type
    create(double pitch) { return pointer_type(new constant(pitch)); }

    /// visitor implementation
    void accept(pitch_visitor& vis) const { vis.visit(*this); }

    /// Virtual destructor.
    virtual ~constant() {}

    /// Pitch, in halftones from a440.
    double pitch;

    private:
    constant(double pitch) : pitch(pitch) {}
};

/**
 * Implements a linear_slide pitch envelope.
 */
struct linear_slide : public pitch {
    typedef std::shared_ptr<const linear_slide> pointer_type;

    /// create new pitch envelopes through this function.
    /// @param pitch the pitch of the note, in halftone offsets from a440.
    /// @return a new linear_slide pitch object.
    static pointer_type
    create(double start_pitch, double end_pitch) {
        return pointer_type(new linear_slide(start_pitch, end_pitch));
    }

    /// visitor implementation
    void accept(pitch_visitor& vis) const { vis.visit(*this); }

    /// Virtual destructor.
    virtual ~linear_slide() {}

    double start_pitch;
    double end_pitch;

    private:
    linear_slide(double start_pitch, double end_pitch)
        : start_pitch(start_pitch), end_pitch(end_pitch)
    {}
};


} /* end namespace pitch */
} /* end namespace notation */
} /* end namespace sgr */

#endif
