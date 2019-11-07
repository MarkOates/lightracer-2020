

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


