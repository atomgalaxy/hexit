#ifndef SGR_NOTATION_HPP
#define SGR_NOTATION_HPP
/**
 * @file notation.hpp
 * Implements the notation namespace.
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-01-31
 */
#include "notation_pitch.hpp"
#include "notation_instrument.hpp"
#include "notation_volume.hpp"
#include "notation_timing.hpp"
#include "units.hpp"

#include <vector>
#include <set>

namespace sgr {
namespace notation {

/** 
 * Represents a duration and accentuation of a single note in a passage.
 */
struct hit {
    /** start, in beats since start of sequence */
    units::beat start;
    /** duration, in beats since start of sequence */
    units::beat duration;
    /** accent, 0-1. May represent volume or some other form of
     * accentuation. */
    double accent;

    hit(decltype(start) start, decltype(duration) duration, decltype(accent) accent)
        : start{start}
        , duration{duration}
        , accent{accent}
    {}

};
typedef std::vector<sgr::notation::hit> rhythm;

/** Holds a single played note. */
struct note {
    instrument::instrument::pointer_type instrument;
    volume::volume::pointer_type         volume;
    pitch::pitch::pointer_type           pitch;
    hit                                  duration;

    note(decltype(instrument) instrument, decltype(volume) volume,
         decltype(pitch) pitch, decltype(duration) duration)
        : instrument{instrument}
        , volume{volume}
        , pitch{pitch}
        , duration{duration}
    {}

};

/**
 * This class enables listening for changes to a song object. This is useful,
 * for instance, if you have a player and one part of the program is producing
 * music in this fashion (song) that the player has to play in real time.
 */
class song_listener {
private:
    song_listener()
        : listeners()
    {}

public:

    struct interface {
        virtual void notify(const note) = 0;
        virtual ~interface() {}
    };

    static std::shared_ptr<song_listener>
    create()
    {
        return std::shared_ptr<song_listener>(new song_listener());
    }


    void notify(const note n)
    {
        for (auto l : listeners) {
            if (l) {
                l->notify(n);
            }
        }
    }

    /* listener pattern support */
    void listen(std::shared_ptr<interface> listener)
    {
        listeners.insert(listener);
    }

    void remove_listener(std::shared_ptr<interface> listener)
    {
        listeners.erase(listener);
    }

    virtual ~song_listener() {}

private:
    /// listeners
    std::set<std::shared_ptr<interface>> listeners;

}; /* end class song_listener */

/**
 * This is supposed to be a lightweight class containing whole songs or just
 * parts of songs. It is designed to be returned from functions etc.
 */
class song {
public:

    song()
        : notes_()
        , timings_()
        , beat_length_(0)
        , listener(nullptr)
    {}

    /* actual song functions */
    song& operator<<(const song& s)
    {
        auto start_of_phrase = beat_length_;
        for (auto t : s.timings_) {
            add_timing(t);
        }
        for (auto n : s.notes_) {
            n.duration.start += start_of_phrase;
            add_note(n);
        }
        return *this;
    }
    song& operator<<(const std::vector<note>& notes_)
    {
        for (auto n : notes_) {
            add_note(n);
        }
        return *this;
    }

    song& operator<<(const note n)
    {
        return add_note(n);
    }

    song& add_note(const note n)
    {
        notes_.push_back(n);
        if (listener) { listener->notify(n); }
        return *this;
    }

    song& add_timing(const timing::timing::pointer_type& t)
    {
        beat_length_ += t->full_beats();
        timings_.push_back(t);
        return *this;
    }
    song& operator<<(const timing::timing::pointer_type& t)
    {
        return add_timing(t);
    }

    const std::vector<note>
    active_notes_(units::beat beat) const
    {
        std::vector<note> instrs;
        for (auto note : notes_) {
            auto start = note.duration.start;
            auto end   = note.duration.start + note.duration.duration;
            if (start >= beat && beat < end) {
                instrs.push_back(note);
            }
        }
        return instrs;
    }

    /* LISTENER SUPPORT */
    void set_listener(std::shared_ptr<song_listener> listener)
    {
        this->listener = listener;
    }
    const std::shared_ptr<song_listener>
    get_or_create_listener()
    {
        if (!listener) {
            listener = song_listener::create();
        }
        return listener;
    }
    /* END LISTENER SUPPORT */

private:
    /// notes_
    std::vector<note> notes_;
    std::vector<timing::timing::pointer_type> timings_;
    units::beat beat_length_;
public:

    const decltype(notes_)& notes()
    {
        return notes_;
    }
    const decltype(timings_)& timings()
    {
        return timings_;
    }

private:
    std::shared_ptr<song_listener> listener;
};

} /* end namespace sgr */
} /* end namespace notation */



#endif
