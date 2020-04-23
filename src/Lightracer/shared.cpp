

#include "Lightracer/shared.hpp"

#include <allegro5/allegro.h>


void fade_to_black(Motion &motion, float &foreground_black_opacity)
{
   float start_time = al_get_time();
   float end_time = start_time + 3.0;
   motion.animate(&foreground_black_opacity, 0.0, 1.0, start_time, end_time);
}


void fade_out_of_black(Motion &motion, float &foreground_black_opacity)
{
   float start_time = al_get_time();
   float end_time = start_time + 3.6;
   motion.animate(&foreground_black_opacity, 1.0, 0.0, start_time, end_time);
}


void flash_white(Motion &motion, float &foreground_white_opacity)
{
   float start_time = al_get_time();
   float end_time = start_time + 2.0;
   motion.animate(&foreground_white_opacity, 1.0, 0.0, start_time, end_time);
}


void play_exit_teleport_sound_effect(ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance)
{
   al_stop_sample_instance(exit_sample_instance);
   al_play_sample_instance(exit_sample_instance);
}


void play_hit_bad()
{
    al_stop_sample_instance(instance_hit_bad);
    al_play_sample_instance(instance_hit_bad);
}


void play_hit_bounce()
{
    al_stop_sample_instance(instance_hit_bounce);
    al_play_sample_instance(instance_hit_bounce);
}


void play_hit_med()
{
    al_stop_sample_instance(instance_hit_med);
    al_play_sample_instance(instance_hit_med);
}


void play_hit_soft()
{
    al_stop_sample_instance(instance_hit_soft);
    al_play_sample_instance(instance_hit_soft);
}


void kill_player(int _segment_where_player_died)
{
   racer->dead = true;
   motion.move_to(&camera->zoom, 0.8, 2, interpolator::slow_in_out);
   //animate_delta(racer->direction_angle, FULL_ROTATION*2, 5.0, interpolator::trippleFastIn);
   al_stop_sample_instance(engine_sample_instance);

   racer->throttle_on = false;
   racer->turning_left = false;
   racer->turning_right = false;
   racer->break_on = false;

   num_lives--;

   if (num_lives <= 0) game_over = true;

   if (_segment_where_player_died != 0) segment_where_player_died.push_back(_segment_where_player_died);

   racer->velocity_magnitude = 0;
   racer->velocity = 0;
}


void complete_track()
{
   //racer->dead = true;
   if (num_of_segments_in_track == 30) game_won = true;
   track_completed = true;
}




ALLEGRO_SAMPLE_INSTANCE *instance_hit_bad = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_bounce = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_med = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_soft = nullptr;
ALLEGRO_SAMPLE_INSTANCE *engine_sample_instance = nullptr;

Track *track = nullptr;
TrackSegment *track_segment = nullptr;
Racer *racer = nullptr;

Motion motion;
CheapCamera *camera = nullptr;

int max_num_lives = 3;
int num_lives = max_num_lives;
bool game_won = false;
bool game_over = false;
bool track_completed = false;
 
vector<int> segment_where_player_died = {};

int num_of_segments_in_track = 4;

