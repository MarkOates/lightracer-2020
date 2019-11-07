#pragma once


#define LEFT_RAIL 1
#define RIGHT_RAIL 2
namespace allegro_flare
{
   typedef char char64_t[64];
}
#define PROFILE_TIMER_TIME_T double


#include "AllegroFlare/Motion.hpp"

using AllegroFlare::Motion;

#include "allegro5/allegro_audio.h"

void fade_to_black(Motion &motion, float &foreground_black_opacity);
void fade_out_of_black(Motion &motion, float &foreground_black_opacity);
void flash_white(Motion &motion, float &foreground_white_opacity);
void play_exit_teleport_sound_effect(ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance);


