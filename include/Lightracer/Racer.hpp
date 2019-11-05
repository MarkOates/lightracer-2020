#pragma once


#include "AllegroFlare/Vec2D.hpp"
#include "AllegroFlare/Motion.hpp" // used for a global animation
#include "vector"


using namespace AllegroFlare; // for vec2d


class Racer
{
public:
   vec2d position, direction, velocity;
   float velocity_magnitude;
   float direction_angle;

   std::vector<double> lap_time;

   float max_health, health;
   int max_boosts;
   int boosts;
   bool throttle_on;
   bool break_on;
   bool turning_right, turning_left;
   bool dead;

   bool using_boost;
   float using_boost_counter;

   Racer();

   void complete_lap();
   void draw();
   void use_boost(Motion &motion, void (*function_to_mark_player_not_using_boost)(void *)); // should be removed
};


