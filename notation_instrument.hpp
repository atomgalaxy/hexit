#ifndef SGR_NOTATION_INSTRUMENT_HPP
#define SGR_NOTATION_INSTRUMENT_HPP
/**
 * @file notation_instrument.hpp
 * Implements the sgr::notation::instruments namespace.
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */

#include <memory>

namespace sgr {
namespace notation {
namespace instrument {

// forward declarations
struct sinewave;
struct sawwave;
struct squarewave;

// visitor needs those forward decls.
struct instrument_visitor {
    virtual void visit(const sinewave& s) = 0;
    virtual void visit(const sawwave& s) = 0;
    virtual void visit(const squarewave& s) = 0;
    virtual ~instrument_visitor() {}
};

struct instrument {
    /// pointer type for instruments
    typedef std::shared_ptr<const instrument> pointer_type;

    /// visitor declaration
    virtual void accept(instrument_visitor& vis) const = 0;

    /// virtual destructor
    virtual ~instrument() {}
};

/* ###########
 * Instruments
 * ########### */

/**
 * This object represents a singe note played as a simple sinewave, with a
 * configurable volume envelope.
 */
struct sinewave : public instrument
{
    typedef std::shared_ptr<const sinewave> pointer_type;

    /**
     * This is how you create a sinewave object, since it always has to be
     * contained in a shared pointer.
     * @return a created sinewave object.
     */
    static pointer_type create() { return pointer_type(new sinewave()); }

    /// visitor interface
    virtual void accept(instrument_visitor& vis) const { vis.visit(*this); }
    /// virtual destructor
    virtual ~sinewave() {}

private:
    sinewave() {}
};

/**
 * An object of this class represents a single note, played as a saw-wave with a
 * configurable volume envelope.
 */
struct sawwave : public instrument
{
    typedef std::shared_ptr<const sawwave> pointer_type;
    /**
     * This is how you create a sawwave object, since it always has to be
     * contained in a shared pointer.
     * @return a created saw-wave object.
     */
    static pointer_type create() { return pointer_type(new sawwave()); }

    /// visitor interface
    virtual void accept(instrument_visitor& vis) const { vis.visit(*this); }
    ///virtual destructor
    virtual ~sawwave() {}
private:
    sawwave() {}
};

/**
 * An object of this class represents a single note, played with a square wave,
 * with a configurable volume envelope.
 */
struct squarewave : public instrument
{
    typedef std::shared_ptr<const squarewave> pointer_type;
    /**
     * This is how you create a squarewave object, since it always has to be
     * contained in a shared pointer.
     * @return a created squarewave object.
     */
    static pointer_type create() { return pointer_type( new squarewave() ); }

    /// visitor interface
    virtual void accept(instrument_visitor& vis) const { vis.visit(*this); }
    /// virtual destructor
    virtual ~squarewave() {}
private:
    squarewave() {}
};


} /* end namespace instruments */
} /* end namespace notation */
} /* end namespace sgr */

#endif
