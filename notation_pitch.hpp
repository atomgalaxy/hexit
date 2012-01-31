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

// visitor
struct pitch_visitor {
    virtual void visit(const constant& p) = 0;
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

    /// returns the pitch of the note.
    double pitch() { return pitch_; }

    /// visitor implementation
    void accept(pitch_visitor& vis) const { vis.visit(*this); }

    /// Virtual destructor.
    virtual ~constant() {}

    private:
    constant(double pitch) : pitch_(pitch) {}

    double pitch_;
};


} /* end namespace pitch */
} /* end namespace notation */
} /* end namespace sgr */

#endif
