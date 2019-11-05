

#include "Lightracer/Racer.hpp"

#include "AllegroFlare/Useful.hpp" // for FULL_ROTATION


Racer::Racer()
{
   //num_laps = 0;
   position = vec2d(100, 100);
   direction = vec2d(1, 0);
   velocity = vec2d(0, 0);
   direction_angle = direction.get_angle();
   direction_angle += FULL_ROTATION/2;
   velocity_magnitude = 0;
   throttle_on = false;
   turning_right = false;
   turning_left = false;
   break_on = false;
   health = 100;
   max_health = 100;
   max_boosts = 3;
   boosts = max_boosts;
   dead = false;
   using_boost = false;
   using_boost_counter = 0.0;
}


void Racer::complete_lap()
{
   //num_laps++;
   //lap_time = al_get_time();
   lap_time.push_back(al_get_time());
}


void Racer::draw()
{
   //vec2d direction_vector;
   al_draw_circle(position.x, position.y, 10, al_color_name("pink"), 2.0);
   al_draw_line(position.x, position.y, position.x+velocity.x*20, position.y+velocity.y*20, al_color_name("pink"), 2.0);
   al_draw_line(position.x, position.y, position.x+direction.x*20, position.y+direction.y*20, al_color_name("yellow"), 2.0);
}


void Racer::use_boost(Motion &motion, void (*function_to_mark_player_not_using_boost)(void *))
{
   if (boosts == 0) { /* do nothing */ }
   if (using_boost) return;

   float start_time = al_get_time();
   float end_time = start_time + 2.0;
   motion.animate(&using_boost_counter, 1.0, 0.0, start_time, end_time, interpolator::double_fast_out, function_to_mark_player_not_using_boost);
   velocity_magnitude = 4.5;

   using_boost = true;
}


