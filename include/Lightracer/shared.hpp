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

#include <vector>
using std::vector;


extern ALLEGRO_SAMPLE_INSTANCE *instance_hit_bad;
extern ALLEGRO_SAMPLE_INSTANCE *instance_hit_bounce;
extern ALLEGRO_SAMPLE_INSTANCE *instance_hit_med;
extern ALLEGRO_SAMPLE_INSTANCE *instance_hit_soft;
extern ALLEGRO_SAMPLE_INSTANCE *engine_sample_instance;


#include "Lightracer/Track.hpp"
#include "Lightracer/TrackSegment.hpp"
#include "Lightracer/Racer.hpp"
#include "Lightracer/CheapCamera.hpp"

extern Track *track;
extern TrackSegment *track_segment;
extern Racer *racer;
extern Motion motion;
extern CheapCamera *camera;

extern int max_num_lives;
extern int num_lives;
extern bool game_won;
extern bool game_over;
extern bool track_completed;

extern int num_of_segments_in_track;

extern vector<int> segment_where_player_died;

void fade_to_black(Motion &motion, float &foreground_black_opacity);
void fade_out_of_black(Motion &motion, float &foreground_black_opacity);
void flash_white(Motion &motion, float &foreground_white_opacity);
void play_exit_teleport_sound_effect(ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance);
void play_hit_bad();
void play_hit_bounce();
void play_hit_med();
void play_hit_soft();
void kill_player(int _segment_where_player_died);

