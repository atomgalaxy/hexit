#ifndef ENGINE_HPP_HEADER
#define ENGINE_HPP_HEADER

/**
 * @file engine.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-04-24
 */

#include "../graphics/mazegenerator.hpp"

#include <osg/Vec2d> #include <string>
#include <memory>

namespace engine {

struct actor_properties {
    osg::Vec2d speed;
    double rotation;
    double attack_damage;
    double health;
}

struct ActiveAttack {
    double time_started;
    double attack_delay;
    double damage;
    std::string target;

    bool attack_now(double time, double epsilon) {
        return (abs(time_started + attack_delay - time) < epsilon);
    }
}

struct actor {
    osg::Vec2d position;
    std::string name;

    actor_properties limits;

    ActiveAttack attack;

    actor(osg::Vec2d position, std::string name)
        : position(position)
        , name(name)
    {}
};

struct StartGoForwardAction  {
    double time;
};
struct StopGoForwardAction  {
    double time;
};
struct StartGoBackwardAction {
    double time;
};
struct StopGoBackwardAction {
    double time;
};
struct StartRotateLeftAction {
    double time;
};
struct StopRotateLeftAction {
    double time;
};
struct StartRotateRightAction {
    double time;
};
struct StopRotateRightAction {
    double time;
};
struct Attack{
    double time;
    std::string targetId;
};



class engine {
    std::map<std::string, actor> actors;
    std::shared_ptr<maze::maze_type> maze;

    double dt;
    double time;

    engine()
        : dt(1./100)
        , time(0)
    {}

    void maze_interface_demo() {
        size_t i = 0, j = 0;
        maze->isWall(i, j);
        maze->isPath(i, j);
        maze->getStart();  // std::pair<size_t, size_t>
        maze->getFinish(); // std::pair<size_t, size_t>
    }

    // moves the simulation forward one tick (0.016 of a second)
    void simulate() {
        time += dt;
        for (auto name_actor_pair : actors) {
            auto& actor = name_actor_pair.second;
            if (maze->isPath((actor.position + actor.speed*dt))) {
                actor.position += actor.speed*dt 
            }
            actor.direction += actor.angularSpeed*dt;
            if (actor.attack.attack_now(time, dt)) {
                // attack damage happens now
                if (actors[actor.attack.target].health > 0){
                    actors[actor.attack.target].health -= actor.attack.damage;
                }
            }
        }
    }

    void addActor(const Actor& actor) {
        actors[actor.name] = actor;
    }

    void applyActionToActor(std::string actorId, GoForwardAction forward)
    {
        auto actor& = actors[actorId];
        actor.speed = actor.properties.speed;
    }
    void applyActionToActor(std::string actorId, GoBackwardAction backward)
    {
        auto actor& = actors[actorId];
        actor.speed = -actor.properties.speed;
    }
    void applyActionToActor(std::string actorId, RotateLeft left)
    {
        auto actor& = actors[actorId];
        actor.rotation = -actor.limits.rotation;
    }
    void applyActionToActor(std::string actorId, RotateRight right)
    {
        auto actor& = actors[actorId];
        actor.rotation = actor.properties.rotation;
    }
    void applyActionToActor(std::string actorId, Attack attack)
    {
        auto target& = actors[attack.targetId];
        auto actor&  = actors[actorId];
        actor.attack = ActiveAttack{time, actor.limits.attack_delay, actor.limits.damage, targetId};
    }

};


}/*end namespace*/

#endif
