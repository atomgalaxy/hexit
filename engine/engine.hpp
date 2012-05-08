#ifndef ENGINE_HPP_HEADER
#define ENGINE_HPP_HEADER

/**
 * @file engine.hpp
 * 
 *
 * @author Gašper Ažman, gasper.azman@gmail.com
 * @since 2012-04-24
 */

#include "../maps/maze.hpp"

#include <osg/Vec2d>
#include <string>
#include <memory>
#include <map>

namespace engine {

static const double TAU = 2*M_PI;

struct actor_properties {
    double speed; // in units per second
    double angular_velocity; // in radians per second

    double attack_damage;
    double attack_delay;

    double health;

    osg::Vec2d
    getSpeedAsVector(double direction) const {
        return osg::Vec2d(cos(direction) * speed, sin(direction) * speed);
    }
};

struct ActiveAttack {
    double time_started;
    double attack_delay;
    double damage;
    std::string target;

    bool is_attack_now(double time, double epsilon) {
        return (abs(time_started + attack_delay - time) < epsilon);
    }
};

struct actor {
    std::string name;
    osg::Vec2d position;
    double direction;

    double speed; // in units per second
    double angular_velocity; // in radians per second

    double attack_damage;
    double attack_delay;

    double health;
    actor_properties limits;
    ActiveAttack attack;

    osg::Vec2d
    getSpeedAsVector() {
        return osg::Vec2d(cos(direction) * speed, sin(direction) * speed);
    }

    actor(std::string name = "",
         osg::Vec2d position = osg::Vec2d(0,0),
         double direction = 0,
         double health = 0,
         actor_properties limits = {}
            )
        : name(name)
        , position(position)
        , direction(direction)
        , speed(1)
        , angular_velocity(0)
        , attack_damage(limits.attack_damage)
        , attack_delay(limits.attack_delay)
        , health(health)
        , limits(limits)
        , attack{
            0,
            0,
            0,
            "",
        }
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
    std::shared_ptr<maps::Maze> maze;

    double dt;
    double time;

    public:
    engine()
        : actors()
        , maze(std::make_shared<maps::Maze>(41, 43, 1))
        , dt(1./100)
        , time(0)
    {}

    actor& getActor(const std::string& actorId) {
        return actors[actorId];
    }

/*     void maze_interface_demo() {
        size_t i = 0, j = 0;
        maze->isWall(i, j);
        maze->isPath(i, j);
        maze->getStart();  // std::pair<size_t, size_t>
        maze->getFinish(); // std::pair<size_t, size_t>
    }
*/
    // moves the simulation forward one tick (0.016 of a second)
    void simulate() {
        time += dt;
        for (auto name_actor_pair : actors) {
            auto& actor = name_actor_pair.second;
            auto endposition =
                actor.position + actor.getSpeedAsVector()*dt;
            if (maze->isPath(
                        endposition.x(), endposition.y()))
            {
                actor.position = endposition;
            }
            actor.direction += actor.angular_velocity*dt;
            if (actor.attack.is_attack_now(time, dt)) {
                // attack damage happens now
                if (actors[actor.attack.target].health > 0){
                    actors[actor.attack.target].health -= actor.attack.damage;
                }
            }
        }
    }
    double getCurrentTime() {
        return time;
    }
    void addActor(const actor& act) {
        actors[act.name] = act;
    }

    void applyActionToActor(
            std::string actorId, StartGoForwardAction forward)
    {
        auto actor  = actors[actorId];
        actor.speed = actor.speed;
    }
    void applyActionToActor(std::string actorId, StopGoForwardAction forward)
    {
        auto actor = actors[actorId];
        actor.speed = 0;
    }
    void applyActionToActor(std::string actorId, StartGoBackwardAction backward)
    {
        auto actor = actors[actorId];
        actor.speed = -actor.speed;
    }
    void applyActionToActor(std::string actorId ,StopGoBackwardAction backward)
    {
        auto actor = actors[actorId];
        actor.speed = 0;
    }
    void applyActionToActor(std::string actorId, StartRotateLeftAction left)
    {
        auto actor = actors[actorId];
        actor.angular_velocity = actor.limits.angular_velocity;
    }
    void applyActionToActor(std::string actorId, StopRotateLeftAction left)
    {
        auto actor = actors[actorId];
        actor.angular_velocity = 0;
    }
    void applyActionToActor(std::string actorId, StartRotateRightAction right)
    {
        auto actor = actors[actorId];
        actor.angular_velocity = actor.angular_velocity;
    }
    void applyActionToActor(std::string actorId, StopRotateRightAction right)
    {
        auto actor = actors[actorId];
        actor.angular_velocity = 0;
    }
    void applyActionToActor(std::string actorId, Attack attack)
    {
        auto& target = actors[attack.targetId];
        auto& actor  = actors[actorId];
        actor.attack = ActiveAttack{
            time,
            actor.attack_delay, actor.attack_damage,
            attack.targetId
        };
    }

};


}/*end namespace*/

#endif
