

#include "Lightracer/RacerTrackCollisionResolver.hpp"


#include <cmath> // for abs
#include "AllegroFlare/Useful.hpp" // for reflect (and maybe something else)


void complete_track();
void flash_white();
void play_hit_bad();
void kill_player(int _segment_where_player_died);
void play_hit_soft();
void play_hit_bounce();
void play_hit_med();


RacerTrackCollisionResolver::RacerTrackCollisionResolver(
        Racer *racer
      , Track *track
      , int &index_of_last_track_segment_that_collides
      , ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance
      , int &num_laps_to_win
      , float &lap_notification_counter
      , Motion &motion
      , float &delay_time_since_last_affect
      )
   : player(*racer)
   , track(track)
   , OMG_DeltaTime(0.6)
   , index_of_last_track_segment_that_collides(index_of_last_track_segment_that_collides)
   , passthough_sample_instance(passthough_sample_instance)
   , num_laps_to_win(num_laps_to_win)
   , lap_notification_counter(lap_notification_counter)
   , motion(motion)
   , delay_time_since_last_affect(delay_time_since_last_affect)
{}


void RacerTrackCollisionResolver::update_player_velocity_and_direction()
{
   static float turn_direction_velocity = 0;
   //if (player.throttle_on) player.velocity_magnitude += 0.065; // good

   if (player.throttle_on) player.velocity_magnitude += 0.085;
   if (player.break_on) player.velocity_magnitude *= 0.9;
   if (player.turning_right) turn_direction_velocity -= 0.014; // good
   if (player.turning_left) turn_direction_velocity += 0.014; // good

   turn_direction_velocity = (turn_direction_velocity * 0.8); // good

   player.direction_angle += turn_direction_velocity * OMG_DeltaTime;

   player.velocity_magnitude *= (1.0-abs(turn_direction_velocity*0.05));// = (turn_direction_velocity * 0.8); // good

   // friction
   //player.velocity_magnitude *= 0.98;
   player.velocity_magnitude *= 0.98; // good

   player.direction = vec2d(sin(player.direction_angle), cos(player.direction_angle));
   player.velocity = (player.velocity*0.6 + player.direction*0.4).normalized() * player.velocity_magnitude;
}


void RacerTrackCollisionResolver::update_racer_along_track()
{
   int __HACK_entrance_already_collided = 0;



   vec2d start_point = vec2d(player.position.x, player.position.y);
   vec2d end_point = start_point + vec2d(player.velocity.x, player.velocity.y) * player.velocity_magnitude * OMG_DeltaTime;


   vec2d player_pos = start_point;
   vec2d player_vel = end_point - start_point;



   float time_left_in_timestep = 1.0;
   int segment_that_collides = 0;
   TrackSegment *track_segment_that_collides = nullptr;
   bool collides_on_left_track_segment = false;
   vec2d point_of_intersection;

   int num_steps = 0;

   bool collides_through_exit = false;

   bool collides_through_entrance = false;
   int entrance_that_collides = 0;

   int track_segment_segment_where_player_collides = 0;

   while(time_left_in_timestep > 0)
   {
      //
      // find the soonest intersection time
      //

      float minimum_collision_time_normal_during_this_pass = 1.0;
      {
         entrance_that_collides = 0;
         minimum_collision_time_normal_during_this_pass = 1.0;

         vec2d __start = player_pos;
         vec2d __end = player_pos+player_vel*time_left_in_timestep;
         LineSegmentInfo motion_segment(__start, __end);

         vec2d &E = motion_segment.from_start;
         vec2d &P1 = motion_segment.perpendicular;
         track_segment_that_collides = nullptr;
         collides_through_exit = false;
         collides_through_entrance = false;

         int track_segment_start = std::max(index_of_last_track_segment_that_collides-2, 0);
         int track_segment_end = std::min(index_of_last_track_segment_that_collides+2, (int)track->segment.size()-1);

         for (int t=track_segment_start; t<(int)(track_segment_end+1); t++)
         {
            TrackSegment *track_segment = track->segment[t];


            // just the left rails first

            for (int i=1; i<(int)track_segment->left_rail.size(); i++)
            {
               vec2d &F = track_segment->left_rail_segments[i]->from_start;
               vec2d &P2 = track_segment->left_rail_segments[i]->perpendicular;

               float h = ((motion_segment.start - track_segment->left_rail_segments[i]->start) * P1) / (F * P1);
               float g = ((track_segment->left_rail_segments[i]->start - motion_segment.start) * P2) / (E * P2);

               if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
               {
                  // there is a collision in this time-step
                  // if it's less than another collision that occured, set the values
                  if (g < minimum_collision_time_normal_during_this_pass)
                  {
                     minimum_collision_time_normal_during_this_pass = g;
                     segment_that_collides = i;
                     point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                     track_segment_that_collides = track_segment;
                     track_segment_segment_where_player_collides = t;
                     collides_on_left_track_segment = true;
                     collides_through_exit = false;
                     collides_through_entrance = false;
                  }
               }
            }


            // now the right rails

            for (int i=1; i<(int)track_segment->right_rail.size(); i++)
            {
               vec2d &F = track_segment->right_rail_segments[i]->from_start;
               vec2d &P2 = track_segment->right_rail_segments[i]->perpendicular;

               float h = ((motion_segment.start - track_segment->right_rail_segments[i]->start) * P1) / (F * P1);
               float g = ((track_segment->right_rail_segments[i]->start - motion_segment.start) * P2) / (E * P2);

               if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
               {
                  // there is a collision in this time-step
                  // if it's less than another collision that occured, set the values
                  if (g < minimum_collision_time_normal_during_this_pass)
                  {
                     minimum_collision_time_normal_during_this_pass = g;
                     segment_that_collides = i;
                     track_segment_segment_where_player_collides = t;
                     point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                     track_segment_that_collides = track_segment;
                     collides_on_left_track_segment = false;
                     collides_through_exit = false;
                     collides_through_entrance = false;
                  }
               }
            }


            // check the exit

            {
               vec2d &F = track->exit_segment_info->from_start;
               vec2d &P2 = track->exit_segment_info->perpendicular;

               float h = ((motion_segment.start - track->exit_segment_info->start) * P1) / (F * P1);
               float g = ((track->exit_segment_info->start - motion_segment.start) * P2) / (E * P2);

               if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
               {
                  // there is a collision in this time-step
                  // if it's less than another collision that occured, set the values
                  if (g < minimum_collision_time_normal_during_this_pass)
                  {
                     collides_through_exit = true;
                     minimum_collision_time_normal_during_this_pass = g;
                     point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                     track_segment_that_collides = nullptr;
                     collides_through_entrance = false;
                  }
               }
            }


            // check the entrance to the next segment

            {
               vec2d &F = track->segment[t]->entrance_segment_info->from_start;
               vec2d &P2 = track->segment[t]->entrance_segment_info->perpendicular;

               float h = ((motion_segment.start - track->segment[t]->entrance_segment_info->start) * P1) / (F * P1);
               float g = ((track->segment[t]->entrance_segment_info->start - motion_segment.start) * P2) / (E * P2);

               if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
               {
                  // there is a collision in this time-step
                  // if it's less than another collision that occured, set the values
                  if (g < minimum_collision_time_normal_during_this_pass)
                  {
                     collides_through_entrance = true;
                     entrance_that_collides = t;
                     collides_through_exit = false;
                     minimum_collision_time_normal_during_this_pass = g;
                     point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                     track_segment_that_collides = nullptr;
                  }
               }
            }
         }
      }


      TrackSegment *terrain_that_collides = track_segment_that_collides;
      bool collides_on_left_terrain = collides_on_left_track_segment;
      int terrain_segment_where_player_collides = track_segment_segment_where_player_collides;
      //
      // move everything by that timestep
      // adjust velocities of colliding objects
      // decrement timestep by amount within timestep
      //
      {

         player_pos += player_vel*time_left_in_timestep*minimum_collision_time_normal_during_this_pass;

         if (collides_through_entrance && (__HACK_entrance_already_collided==0))
         {
            __HACK_entrance_already_collided++;
            player_pos += player_vel * 0.01; // < this is a solution that causes a *rare* *rare* bug where the player
                                     // can escape the track by colliding precicely with the segment entrance and wall
                                     // 

               index_of_last_track_segment_that_collides = entrance_that_collides;
               al_stop_sample_instance(passthough_sample_instance);
               al_play_sample_instance(passthough_sample_instance);
         }

         if (collides_through_exit)
         {
            index_of_last_track_segment_that_collides = 0;

            player_pos = (track->enter_p2 - track->enter_p1) / 2 + track->enter_p1;
            
            player.complete_lap();

            if ((int)player.lap_time.size() >= num_laps_to_win) complete_track();

            float start_time = al_get_time();
            float end_time = start_time + 3.0;
            motion.animate(&lap_notification_counter, 1.0, 0.0, start_time, end_time, interpolator::quadruple_fast_out);

            flash_white();
            
            // rotate velocity and direction
            player.direction_angle += track->exit_segment_info->from_start.get_angle();
            player_vel = 
               vec2d::polar_coords(player.velocity.get_angle() - track->exit_segment_info->from_start.get_angle(), 1);
         }
         else if (terrain_that_collides != nullptr) 
         {
            if (collides_on_left_terrain)
            {
               player_pos += terrain_that_collides->left_rail_segments[segment_that_collides]->normal * 0.1;
               player_vel = reflect(player_vel, terrain_that_collides->left_rail_segments[segment_that_collides]->normal);
            }
            if (!collides_on_left_terrain)
            {
               player_pos += terrain_that_collides->right_rail_segments[segment_that_collides]->normal * 0.1;
               player_vel = reflect(player_vel, terrain_that_collides->right_rail_segments[segment_that_collides]->normal);
            }



            switch(terrain_that_collides->color_type)
            {
            case COLOR_TYPE_RED:
               player.velocity_magnitude *= 0.95;
               if (delay_time_since_last_affect < 0)
               {
                  play_hit_bad();
                  delay_time_since_last_affect = 1.0f;
                  player.health -= 20;
               }
               if (player.health < 0) { player.health = 0; kill_player(terrain_segment_where_player_collides); }
               break;

            case COLOR_TYPE_RED_DEATH:
               player.velocity_magnitude *= 0.95;
               player.health = 0;
               play_hit_bad();
               kill_player(terrain_segment_where_player_collides);
               break;

            case COLOR_TYPE_GREEN:
               if (delay_time_since_last_affect < 0)
               {
                  delay_time_since_last_affect = 1.0f;
                  player.health += 10.0;
                  play_hit_soft();
               }
               if (player.health > player.max_health) player.health = player.max_health;
               break;

            case COLOR_TYPE_BLUE: // bouncy
               if (delay_time_since_last_affect < 0)
               {
                  play_hit_bounce();
                  delay_time_since_last_affect = 1.0f;
                  player.velocity_magnitude = 4.5;
               }
               break;

            case COLOR_TYPE_YELLOW:
               player.velocity_magnitude *= 0.95;
               if (delay_time_since_last_affect < 0)
               {
                  play_hit_med();
                  delay_time_since_last_affect = 1.0f;
                  player.health -= 4;
                  if (player.health < 0) { player.health = 0; kill_player(terrain_segment_where_player_collides); }
               }
               break;

            default:       
               player.velocity_magnitude *= 0.95;
               break;
            }
         }
      }

      time_left_in_timestep -= (time_left_in_timestep*minimum_collision_time_normal_during_this_pass);
      num_steps++;
   }
   
   player.position.x = player_pos.x;
   player.position.y = player_pos.y;
   player.velocity.x = player_vel.x;
   player.velocity.y = player_vel.y;
}


void RacerTrackCollisionResolver::resolve()
{
   update_player_velocity_and_direction();
   update_racer_along_track(); // < as I understand, *everything* that OMG_DeltaTime affects is in here
}


