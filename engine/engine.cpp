/**
 * @file engine.cpp
 *  TEST FILE FOR THE ENGINE
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-04-24
 */

#include "engine.hpp"
#include <cassert>
#include <cmath>

static const double TAU = 2*M_PI;

int main( int argc, char *argv[] )
{
    engine::engine e;
    e.addActor(
            engine::actor(
                "mojca", //name
                osg::Vec2d(1,1), //position
                0, // direction
                60, // health
                engine::actor_properties{
                    1, //speed
                    TAU/2.0, //angular vel
                    15, // attack damage
                    0.5, // attack delay
                    60
                }));
    e.applyActionToActor("mojca",
            engine::StartGoForwardAction{
                e.getCurrentTime()
                }
            );
    // add actors
    // add some events
    // simulate the shit out of it
    for (size_t i = 0; i < 1000; ++i) {
        e.simulate();
    }

    e.applyActionToActor("mojca",
            engine::StopGoForwardAction{
                e.getCurrentTime()
            }
            );

    // check if everybody is where he/she is supposed to be
    assert(abs((e.getActor("mojca").position - osg::Vec2d(11,1)).length()) < 0.1);

    return EXIT_SUCCESS;
}               /* --------  end of function main  ---------- */
