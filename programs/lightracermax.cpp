bool development = false;


#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_native_dialog.h>

#include "AllegroFlare/FontBin.hpp"
#include "AllegroFlare/SampleBin.hpp"
#include "AllegroFlare/Useful.hpp"
#include "AllegroFlare/Vec2D.hpp"
using AllegroFlare::vec2d;
//#include "position2d.h"
//#include "object2d.h"
#include "AllegroFlare/Interpolators.hpp"
#include "AllegroFlare/Motion.hpp"



#define SCREEN_W 1920
#define SCREEN_H 1080
#define SCREEN_HW (SCREEN_W/2)
#define SCREEN_HH (SCREEN_H/2)

#include "allegro_flare/profile_timer.h"
using namespace allegro_flare;


using namespace AllegroFlare;
Motion motion;


#include <vector>
#include <cmath>


using std::vector;



#include <AllegroFlare/BitmapBin.hpp>
using AllegroFlare::BitmapBin;
BitmapBin bitmaps;


#include <AllegroFlare/FontBin.hpp>
using AllegroFlare::FontBin;
FontBin fonts;


#include <AllegroFlare/SampleBin.hpp>
using AllegroFlare::SampleBin;
SampleBin samples;


#include <AllegroFlare/Timer.hpp>
#include <AllegroFlare/TimerFormatter.hpp>
using AllegroFlare::TimerFormatter;
AllegroFlare::Timer stopwatch;



bool logo_showing = true;
float logo_scale = 0.0;


float delay_time_since_last_affect = 1.0;



// restart music at beginning of thing

// longer music

// remove extra time at end of music

// exit gate sound




ALLEGRO_SAMPLE_INSTANCE *instance_hit_bad = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_bounce = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_med = nullptr;
ALLEGRO_SAMPLE_INSTANCE *instance_hit_soft = nullptr;



ALLEGRO_BITMAP *black_screen_overlay = nullptr;



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



ALLEGRO_SAMPLE_INSTANCE *engine_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *music_sample_instance = nullptr;
ALLEGRO_SAMPLE_INSTANCE *exit_sample_instance = nullptr;
ALLEGRO_SAMPLE *music_sample = nullptr;

ALLEGRO_VOICE *engine_voice = nullptr;
ALLEGRO_MIXER *engine_mixer = nullptr;
bool FLAG_draw_profile_graph = false;

ALLEGRO_COLOR start_text_color;




#include "Lightracer/PlayerStats.hpp"




int max_num_lives = 3;
int num_lives = max_num_lives;


bool game_won = false;
bool game_over = false;

bool final_course = false;


float lap_notification_counter = 0.0;
float track_begin_notification_counter = 0.0;


void init_black_screen_overlay()
{
   ALLEGRO_STATE previous_bitmap_state;
   al_store_state(&previous_bitmap_state, ALLEGRO_STATE_TARGET_BITMAP);

   black_screen_overlay = al_create_bitmap(SCREEN_W/3, SCREEN_H/3);
   al_set_target_bitmap(black_screen_overlay);
   al_clear_to_color(al_color_name("white"));

   al_restore_state(&previous_bitmap_state);
}


void draw_black_screen_overlay(ALLEGRO_COLOR color)
{
   static int NO_FLAGS = 0;
   al_draw_tinted_scaled_bitmap(black_screen_overlay, color,
      0, 0, al_get_bitmap_width(black_screen_overlay), al_get_bitmap_height(black_screen_overlay),
      0, 0, SCREEN_W, SCREEN_H, NO_FLAGS);
}



void restart_music()
{
   if (!music_sample) return;
   al_stop_sample_instance(music_sample_instance);
   al_play_sample_instance(music_sample_instance);
}


std::string get_number_string(int num)
{
   switch(num)
   {
   case 1:
      return "one";
      break;
   case 2:
      return "two";
      break;
   case 3:
      return "three";
      break;
   case 4:
      return "four";
      break;
   case 5:
      return "five";
      break;
   case 6:
      return "six";
      break;
   case 7:
      return "seven";
      break;
   case 8:
      return "eight";
      break;
   case 9:
      return "nine";
      break;
   case 10:
      return "ten";
      break;
   case 11:
      return "eleven";
      break;
   default: return tostring(num);
      break;
   }
}





//int segment_where_player_died = 0;

vector<int> segment_where_player_died;



void start_track_begin_text()
{
   float start_time = al_get_time();
   float end_time = start_time + 4.0;
   motion.animate(&track_begin_notification_counter, 1.0, 0.0, start_time, end_time, interpolator::tripple_fast_out);
}


int num_of_segments_in_track = 4;



float finish_track_animation_counter = 1.0;
bool track_completed = false;

int num_laps_to_win = 4;

float foreground_black_opacity = 1.0;
float foreground_white_opacity = 0.0;


int index_of_last_track_segment_that_collides = 0;




void fade_to_black()
{
   float start_time = al_get_time();
   float end_time = start_time + 3.0;
   motion.animate(&foreground_black_opacity, 0.0, 1.0, start_time, end_time);
}

void fade_out_of_black()
{
   float start_time = al_get_time();
   float end_time = start_time + 3.6;
   motion.animate(&foreground_black_opacity, 1.0, 0.0, start_time, end_time);
}

void flash_white()
{
   float start_time = al_get_time();
   float end_time = start_time + 2.0;
   motion.animate(&foreground_white_opacity, 1.0, 0.0, start_time, end_time);
   al_stop_sample_instance(exit_sample_instance);
   al_play_sample_instance(exit_sample_instance);
}



#include "Lightracer/CheapCamera.hpp"



CheapCamera *camera = nullptr;



#include "Lightracer/Camera.hpp"



Camera *good_camera = nullptr;


void zoom_way_out()
{
   motion.move_to(&camera->zoom, 0.05, 0.4);
}

void zoom_normal()
{
   motion.move_to(&camera->zoom, 1.0, 0.4);
}



#include "Lightracer/LineSegmentInfo.hpp"





#include "Lightracer/TrackSegment.hpp"


// track construction functions //


#include "Lightracer/TrackFactory.hpp"


#include "Lightracer/Track.hpp"


void complete_track();
void mark_player_not_using_boost(void *);


#include "Lightracer/Racer.hpp"



#include "Lightracer/Hud.hpp"




// globuals //

Track *track = nullptr;
TrackSegment *track_segment = nullptr;
Racer *racer = nullptr;



void mark_player_not_using_boost(void *)
{
   racer->using_boost = false;
}






//#include "image_bin.h"

float get_zoom_scale(float const &world_z, CheapCamera *cam)
{
    float distance = cam->z - world_z;
   if (distance <= 0) return 0;
   if (cam->depth <= 0) return 0;

    return cam->depth / distance; 
}

inline float scale(float &val)
{
   return val;
}



inline void rotate_point(vec2d *point, const vec2d &axis, float angle)
{
   const float c = cosf(-angle * ALLEGRO_PI / 180);
   const float s = sinf(-angle * ALLEGRO_PI / 180);

   point->x -= axis.x;
   point->y -= axis.y;
   
   const float tx = point->x;
   
   point->x = c * tx - s * point->y;
   point->y = s * tx + c * point->y;
   
   point->x += axis.x;
   point->y += axis.y;
}


inline void project_point(ALLEGRO_VERTEX &v, CheapCamera *cam, float multiplier=0.1)
{
   //if (v.y < 0) v.y = 0;
   //v.x -= cam->x;
   v.x -= cam->x;
   v.y -= cam->y;
   vec2d p(v.x, v.y);
   p = rotate_point(p, cam->rotation + FULL_ROTATION/2);
   v.x = p.x;
   v.y = p.y;
   //v.x *= get_zoom_scale(v.y, cam);
   v.x *= get_zoom_scale(v.y*multiplier, cam);
}





// hud //

void draw_text_with_letter_spacing(int font_size, ALLEGRO_COLOR color, int x, int y, float letter_spacing, ALLEGRO_FONT *font, std::string string_to_write)
{
   //ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf", font_size);
   //ALLEGRO_COLOR color = al_color_name("white");

   std::string the_char = " ";
   int num_characters = string_to_write.length();
   int letters_traversed = 0;
   int leftmost_character_x = SCREEN_HW - (num_characters * letter_spacing) * 0.5;
   for (auto &c : string_to_write)
   {
      the_char[0] = c;
      al_draw_text(font, color, leftmost_character_x + (letters_traversed * letter_spacing), y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
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

// game calculation functions //





#include <algorithm>



#include "Lightracer/RacerTrackCollisionResolver.hpp"





void start_track()
{
   float start_time = al_get_time();
   float end_time = start_time + 3.0;
   motion.animate(&camera->zoom, 0.2, 1.0, start_time, end_time, interpolator::double_slow_in_out);
   racer->position = vec2d(50, -50);
   racer->direction_angle = FULL_ROTATION/2;
   racer->velocity_magnitude = 0;
   racer->lap_time.clear();
   track->start_time = al_get_time();
   al_play_sample_instance(engine_sample_instance);

   racer->health = racer->max_health;
   racer->dead = false;
   track_completed = false;

   logo_showing = false;

   index_of_last_track_segment_that_collides = 0;

   track->make_segments_easy_where_player_died();

   start_track_begin_text();
   stopwatch = AllegroFlare::Timer();
   stopwatch.start();

   fade_out_of_black();

}



void complete_track()
{
   //racer->dead = true;
   if (num_of_segments_in_track == 30) game_won = true;
   track_completed = true;
}




// core and orginazation functions //

void init_game()
{
   init_black_screen_overlay();

   std::cout << "start of bitmap bin path setting" << std::endl;
   bitmaps.set_path("data/images");
   samples.set_path("data/sounds");
   fonts.set_path("data/fonts");
   std::cout << "end of bitmap bin path setting" << std::endl;
   float start_time = al_get_time();
   float end_time = start_time + 7.0;
   motion.animate(&logo_scale, 0.7, 1.0, start_time, end_time, interpolator::tripple_fast_in);

   start_text_color = al_color_name("white");

   engine_voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
   engine_mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);
   al_attach_mixer_to_voice(engine_mixer, engine_voice);

   engine_sample_instance = al_create_sample_instance(samples["engine.ogg"]);
   al_attach_sample_instance_to_mixer(engine_sample_instance, engine_mixer);
   al_set_sample_instance_playmode(engine_sample_instance, ALLEGRO_PLAYMODE_LOOP);
   al_set_sample_instance_gain(engine_sample_instance, 0.0);
   al_set_sample_instance_speed(engine_sample_instance, 0.8);

   music_sample = al_load_sample("data/sounds/LightRacer music - 01.ogg");
   if (music_sample)
   {
      start_text_color = al_color_name("dodgerblue");
      music_sample_instance = al_create_sample_instance(music_sample);
      al_attach_sample_instance_to_mixer(music_sample_instance, engine_mixer);
      al_set_sample_instance_playmode(music_sample_instance, ALLEGRO_PLAYMODE_LOOP);
      al_set_sample_instance_gain(music_sample_instance, 1.0);
      al_set_sample_instance_speed(music_sample_instance, 1.0);
   }

   //al_play_sample_instance(engine_sample_instance);

   // gate pass-through sound

   passthough_sample_instance = al_create_sample_instance(samples["click.ogg"]);
   al_attach_sample_instance_to_mixer(passthough_sample_instance, engine_mixer);
   al_set_sample_instance_playmode(passthough_sample_instance, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(passthough_sample_instance, 1.0);
   al_set_sample_instance_speed(passthough_sample_instance, 1.0);

   // exit pass-through sound

   exit_sample_instance = al_create_sample_instance(samples["through_gate.ogg"]);
   al_attach_sample_instance_to_mixer(exit_sample_instance, engine_mixer);
   al_set_sample_instance_playmode(exit_sample_instance, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(exit_sample_instance, 0.7);
   al_set_sample_instance_speed(exit_sample_instance, 1.0);
   //al_play_sample_instance(exit_sample_instance);




   instance_hit_bad = al_create_sample_instance(samples["hit_bad.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_bad, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_bad, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_bad, 0.7);
   al_set_sample_instance_speed(instance_hit_bad, 1.0);
   //al_play_sample_instance(exit_sample_instance);

   instance_hit_bounce = al_create_sample_instance(samples["hit_bounce.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_bounce, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_bounce, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_bounce, 0.7);
   al_set_sample_instance_speed(instance_hit_bounce, 1.0);
   //al_play_sample_instance(exit_sample_instance);


   instance_hit_med = al_create_sample_instance(samples["hit_med2.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_med, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_med, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_med, 0.7);
   al_set_sample_instance_speed(instance_hit_med, 1.0);
   //al_play_sample_instance(exit_sample_instance);

   instance_hit_soft = al_create_sample_instance(samples["hit_soft.ogg"]);
   al_attach_sample_instance_to_mixer(instance_hit_soft, engine_mixer);
   al_set_sample_instance_playmode(instance_hit_soft, ALLEGRO_PLAYMODE_ONCE);
   al_set_sample_instance_gain(instance_hit_soft, 0.7);
   al_set_sample_instance_speed(instance_hit_soft, 1.0);
   //al_play_sample_instance(exit_sample_instance);

   

   //al_play_sample(samples["engine"), 0.5, 0.5, 1.0, ALLEGRO_PLAYMODE_LOOP, nullptr);

   track = new Track();
   create_random_track(segment_where_player_died, track, num_of_segments_in_track);
   start_track_begin_text();

   restart_music();

   racer = new Racer();

   camera = new CheapCamera();
   good_camera = new Camera(SCREEN_W, SCREEN_H);

   fade_out_of_black();

   //start_track();
}



void destroy_game()
{
   std::cout << CONSOLE_COLOR_YELLOW << "Shutting down game..." << CONSOLE_COLOR_DEFAULT << std::endl;
   samples.clear();
   fonts.clear();
   bitmaps.clear();
   std::cout << CONSOLE_COLOR_YELLOW << "...done." << CONSOLE_COLOR_DEFAULT << std::endl;
}



void key_char_func(ALLEGRO_EVENT *current_event)
{
   //z.update();

   //if (z.get_num_active_animations() != 0) debug_message("camera on dolly");

   //debug_message("Camera (" + tostring(x) + ", " + tostring(y) + ", " + tostring(z) + ")");
   //debug_message("Camera Depth (" + tostring(depth) + ")");

   switch(current_event->keyboard.keycode)
   {
      case ALLEGRO_KEY_PAD_6: good_camera->x += 2.5f; break;
      case ALLEGRO_KEY_PAD_4: good_camera->x -= 2.5f; break;
      case ALLEGRO_KEY_PAD_8: good_camera->y -= 2.5f; break;
      case ALLEGRO_KEY_PAD_2: good_camera->y += 2.5f; break;

      case ALLEGRO_KEY_PAD_PLUS: good_camera->z += 0.5f; break;
      case ALLEGRO_KEY_PAD_MINUS: good_camera->z -= 0.5f; break;
      case ALLEGRO_KEY_PAD_ASTERISK: good_camera->depth -= 1.0f; break;
      case ALLEGRO_KEY_PAD_SLASH: good_camera->depth += 1.0f; break;

      //if (just_pressed(ALLEGRO_KEY_F2: draw_objects_in_3D_space = !draw_objects_in_3D_space;
   }
}


void racer__on_key_up(ALLEGRO_EVENT *current_event)
{
   if (racer->dead) return;

   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_UP:
      racer->throttle_on = false;
      break;
   case ALLEGRO_KEY_LEFT:
      racer->turning_left = false;
      break;
   case ALLEGRO_KEY_RIGHT:
      racer->turning_right = false;
      break;
   case ALLEGRO_KEY_DOWN:
      racer->break_on = false;
      break;
   }
}


void racer__on_key_down(ALLEGRO_EVENT *current_event)
{
   if (racer->dead) return;

   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_UP:
      racer->throttle_on = true;
      break;
   case ALLEGRO_KEY_LEFT:
      racer->turning_left = true;
      break;
   case ALLEGRO_KEY_RIGHT:
      racer->turning_right = true;
      break;
   case ALLEGRO_KEY_DOWN:
      racer->break_on = true;
      break;
   case ALLEGRO_KEY_SPACE:
      //use_boost(motion, mark_player_not_using_boost);
      break;
   }
}



void key_up_func(ALLEGRO_EVENT *current_event)
{
   racer__on_key_up(current_event);
}


#include "AllegroFlare/Framework.hpp"



void key_down_func(Framework &framework, ALLEGRO_EVENT *current_event)
{
   racer__on_key_down(current_event);

   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_ESCAPE:
      framework.shutdown_program = true;
      break;
   case ALLEGRO_KEY_ENTER:
      if (game_over || game_won)
      {
         delete track;
         track = new Track();
         game_won = false;
         final_course = false;
         game_over = false;
         num_lives = max_num_lives;
         num_of_segments_in_track = 4;
         create_random_track(segment_where_player_died, track, num_of_segments_in_track);
      }
      if (track_completed)
      {
         delete track;
         track = new Track();
         num_of_segments_in_track += 6;
         if (num_of_segments_in_track > 30)
         {
            num_of_segments_in_track = 30;
            final_course = true;
         }
         create_random_track(segment_where_player_died, track, num_of_segments_in_track);
      }
      start_track();
      break;
   }
}


/*
void DEVELOPER_key_down_func()
{
   switch(current_event->keyboard.keycode)
   {
   case ALLEGRO_KEY_ENTER:
      if (track_completed)
      {
         delete track;
         track = new Track();
         num_of_segments_in_track += 6;
         create_random_track(track, num_of_segments_in_track);
      }
      start_track();
      break;
      //zoom_way_out();
      break;
   case ALLEGRO_KEY_PGDN:
      animate_to(camera->zoom, 0.5); 
      break;
   case ALLEGRO_KEY_HOME:
      zoom_normal();
      break;
   case ALLEGRO_KEY_E:
      delete track;
      track = new Track();
      create_random_track(track, 0);
      start_track();
      break;
   case ALLEGRO_KEY_N:
      delete track;
      track = new Track();
      create_random_track(track, num_of_segments_in_track);
      start_track();
      break;
   case ALLEGRO_KEY_K:
      kill_player(0);
      break;
   case ALLEGRO_KEY_F1:
      FLAG_draw_profile_graph = !FLAG_draw_profile_graph;
      break;
   case ALLEGRO_KEY_S:
      start_track();
      break;
   case ALLEGRO_KEY_OPENBRACE:
      OMG_DeltaTime -= 0.1;
      break;
   case ALLEGRO_KEY_CLOSEBRACE:
      OMG_DeltaTime += 0.1;
      break;
   }
}
*/


void main_menu_timer_func(ALLEGRO_EVENT *event)
{
   float screen_center_x = SCREEN_HW;
   float screen_center_y = SCREEN_HH;

   ALLEGRO_BITMAP *logo_img = bitmaps.auto_get("lightracer-max-logo-02.png");
   ALLEGRO_COLOR logo_fade_opacity = al_map_rgba_f(1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity);
   al_draw_tinted_scaled_rotated_bitmap(logo_img, logo_fade_opacity, al_get_bitmap_width(logo_img)/2, al_get_bitmap_height(logo_img)/2,
      screen_center_x, (250 - 300) + screen_center_y, logo_scale, logo_scale, 0, 0);

   int font_size = -25;
   draw_text_with_letter_spacing(font_size, al_color_name("white"), screen_center_x, (375 - 300) + screen_center_y + 20, 12*2, fonts["venus_rising_rg.ttf 25"], "press ANY KEY to BEGIN");
   //al_draw_text(fonts["venus_rising_rg.ttf", -25), al_color_name("white"),
      //screen_center_x, (375 - 300) + screen_center_y + 20, ALLEGRO_ALIGN_CENTRE, "press ANY KEY to BEGIN");

   //draw_black_screen_overlay(al_map_rgba_f(0, 0, 0, foreground_black_opacity));
}



void game_timer_func(ALLEGRO_EVENT *current_event)
{
   float screen_center_x = SCREEN_HW;
   float screen_center_y = SCREEN_HH;

   static ALLEGRO_FONT *font_regular = fonts["venus_rising_rg.ttf 28"];
   static ALLEGRO_FONT *font_large = fonts["venus_rising_rg.ttf 50"];



   delay_time_since_last_affect -= 0.2;
   if (delay_time_since_last_affect < 0) delay_time_since_last_affect = -0.1f;




   start_profile_timer("racer/track");
   RacerTrackCollisionResolver collision_resolver(
        racer
      , track
      , index_of_last_track_segment_that_collides
      , passthough_sample_instance
      , num_laps_to_win
      , lap_notification_counter
      , motion
      , delay_time_since_last_affect
      );
   collision_resolver.resolve();
   stop_profile_timer("racer/track");


   camera->x = racer->position.x;
   camera->y = racer->position.y;
   camera->rotation = racer->direction_angle;
   camera->transform_on();


   good_camera->x = racer->position.x;
   good_camera->z = racer->position.y;

   al_set_sample_instance_speed(engine_sample_instance, 0.8+(racer->velocity_magnitude*racer->velocity_magnitude)/7);
   al_set_sample_instance_gain(engine_sample_instance, 0.6);

   start_profile_timer("draw projected");
   track->draw_projected(
         index_of_last_track_segment_that_collides, racer, bitmaps, good_camera, camera,
         racer->direction_angle, racer->position.x, racer->position.y, racer->velocity_magnitude);
   stop_profile_timer("draw projected");


   int current_lap_num = (int)racer->lap_time.size()+1;
   int current_racer_health = racer->health;
   int current_racer_max_health = racer->max_health;
   Hud(num_of_segments_in_track, current_racer_health, current_racer_max_health, current_lap_num, num_laps_to_win, fonts, SCREEN_W, stopwatch, num_lives).draw();


   ALLEGRO_TRANSFORM ident;
   al_identity_transform(&ident);
   al_use_transform(&ident);

   //if (foreground_black_opacity >= 0.01) draw_black_screen_overlay(al_map_rgba_f(0, 0, 0, foreground_black_opacity));

   //ALLEGRO_COLOR __foreground_color = al_map_rgba_f(foreground_white_opacity, foreground_white_opacity, foreground_white_opacity, foreground_white_opacity);
   //draw_black_screen_overlay(__foreground_color);


   if (game_won)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "YOU WIN");
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "congratulations and");
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 300-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "thanks for playing!");
   }
   else if (game_over)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("red"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
      al_draw_text(font_regular, al_color_name("red"), screen_center_x, 350-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to START A NEW GAME");
   }
   else if (racer->dead)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("orange"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "YOU FAILED");
      al_draw_text(font_regular, al_color_name("orange"), screen_center_x, 350-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
   }
   else
   {
      if (lap_notification_counter != 0 && ((racer->lap_time.size()) != num_laps_to_win))
      {

         std::string lap_string = "LAP " + tostring(racer->lap_time.size()+1);
         if ((int)racer->lap_time.size() == (num_laps_to_win-1)) lap_string = "FINAL LAP";


         int font_size = -30;
         draw_text_with_letter_spacing(font_size, al_color_name("white"), 0, screen_center_y - 160, 40 + 30 * (1.0 - lap_notification_counter), fonts["venus_rising_rg.ttf 30"], lap_string.c_str());

         //al_draw_text(font_large, al_map_rgba_f(lap_notification_counter, lap_notification_counter, lap_notification_counter, lap_notification_counter),
            //screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_string.c_str());
      }

      if (track_begin_notification_counter > 0.4)
      {
         std::string lap_string = "READY";
         if (final_course) lap_string = "==-== FINAL COURSE ==-==";
         std::string lap_info_string = "track with " + tostring(num_of_segments_in_track) + " segments";
         std::string lap_info2_string = "with " + get_number_string(segment_where_player_died.size()) + " trouble spot";
         if (segment_where_player_died.size()==1) lap_info2_string += " removed";
         else lap_info2_string += "s removed";

         if (fmod(track_begin_notification_counter, 0.1f) < 0.05)
         {
            //ALLEGRO_COLOR color = al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter);
            int font_size = -30;
            draw_text_with_letter_spacing(font_size, al_color_name("white"), 0, 250-300 + screen_center_y, 50, fonts["venus_rising_rg.ttf 30"], lap_string.c_str());

            //al_draw_text(font_large, al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
               //screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_string.c_str());
         }


         draw_text_with_letter_spacing(-30, al_color_name("white"), screen_center_x, screen_center_y, 50, fonts["venus_rising_rg.ttf 30"], lap_info_string.c_str());

         //al_draw_text(font_large, al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
            //screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_info_string.c_str());

         if (!segment_where_player_died.empty())
            al_draw_text(font_regular, al_map_rgba_f(track_begin_notification_counter*0.5, track_begin_notification_counter, 0.0, track_begin_notification_counter),
               screen_center_x, 300-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_info2_string.c_str());
      }
      else if (track_begin_notification_counter < 0.4 && (!(racer->velocity_magnitude > 0.01)))
      {
         std::string go_string = "GO";

         float opacity_counter = track_begin_notification_counter/0.2;

         al_draw_text(font_large, al_map_rgba_f(0.0, 1.0*opacity_counter, 0.0, opacity_counter),
            screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, go_string.c_str());
      }

      if (track_completed)
      {
         stopwatch.pause();
         al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "TRACK COMPLETED");
         al_draw_text(font_regular, al_color_name("dodgerblue"), screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
         racer->velocity_magnitude *= 0.98;
         racer->throttle_on = false;
      }



   }


}



#include "Lightracer/Camera3.hpp"



void setup_projection_SCENE(Camera3 &camera_to_use, ALLEGRO_BITMAP *backbuffer_sub_bitmap, ALLEGRO_TRANSFORM *transform_to_fill=NULL)
{
   // setup the render settings
   al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
   al_set_render_state(ALLEGRO_WRITE_MASK, ALLEGRO_MASK_DEPTH | ALLEGRO_MASK_RGBA);
   al_clear_depth_buffer(1);


   ALLEGRO_TRANSFORM t;

   camera_to_use.reverse_position_transform(&t);

   float aspect_ratio = (float)al_get_bitmap_height(backbuffer_sub_bitmap) / al_get_bitmap_width(backbuffer_sub_bitmap);
   al_perspective_transform(&t, -1, aspect_ratio, 1, 1, -aspect_ratio, 100);
//	al_perspective_transform(&t, -1, aspect_ratio, 4, 1, -aspect_ratio, 100);
/*
   float w = al_get_bitmap_width(backbuffer_sub_bitmap);
//	   al_orthographic_transform(&t, -1*w, aspect_ratio*w, -100, -1*w, -aspect_ratio*w, 1000);
   ALLEGRO_BITMAP *bitmap = backbuffer_sub_bitmap;
//al_orthographic_transform(&t, -al_get_bitmap_width(bitmap), -al_get_bitmap_height(bitmap), -100.0, al_get_bitmap_width(bitmap),
                       //al_get_bitmap_height(bitmap), 100.0);

   //al_scale_transform_3d(&t, al_display_width(bitmap), al_display_height(bitmap));
   al_scale_transform_3d(&t, 150, 150, 1); 
*/		
/*
al_orthographic_transform(&t, -al_get_bitmap_width(bitmap), al_get_bitmap_height(bitmap), 30.0, al_get_bitmap_width(bitmap),
                       -al_get_bitmap_height(bitmap), -30.0);
*/
   if (transform_to_fill != NULL)
   {
      al_copy_transform(transform_to_fill, &t);
   }

   //al_set_target_bitmap(backbuffer_sub_bitmap); << I don't think this is necessairy, it also occours just prior to this function
   al_use_projection_transform(&t);
}



#include "allegro_flare/placement3d.h"


void draw_gl_projection(Camera3 &camera3, Racer *racer, ALLEGRO_BITMAP *bitmap, ModelBin &models, Track *track_)
{
   setup_projection_SCENE(camera3, bitmap, NULL);

   Model3D &cube_model = *models["rounded_unit_cube-01.obj"];
   cube_model.draw();

   placement3d place;
   float multiplier = 0.01;

   if (track_)
   {
      vec3d exit_p1_pos = vec3d(track->exit_p1.x * multiplier, 0, track->exit_p1.y * multiplier);
      vec3d exit_p2_pos = vec3d(track->exit_p2.x * multiplier, 0, track->exit_p2.y * multiplier);

      place.position = exit_p1_pos;
      place.start_transform();
      cube_model.draw();
      place.restore_transform();

      place.position = exit_p2_pos;
      place.start_transform();
      cube_model.draw();
      place.restore_transform();

      vec3d enter_p1_pos = vec3d(track->enter_p1.x * multiplier, 0, track->enter_p1.y * multiplier);
      vec3d enter_p2_pos = vec3d(track->enter_p2.x * multiplier, 0, track->enter_p2.y * multiplier);

      place.position = enter_p1_pos;
      place.start_transform();
      cube_model.draw();
      place.restore_transform();

      place.position = enter_p2_pos;
      place.start_transform();
      cube_model.draw();
      place.restore_transform();

   }
   
   // draw the racer

   if (racer)
   {
      vec3d racer_pos = vec3d(racer->position.x * multiplier, 0, racer->position.y * multiplier);
      place.position = racer_pos;
      place.start_transform();
      cube_model.draw();
      place.restore_transform();
   }
}



#include "AllegroFlare/Framework.hpp"
#include "AllegroFlare/Screen.hpp"


using Lightracer::PlayerStats;


class LightracerMax : public Screen
{
public:
   ALLEGRO_BITMAP *sub_bitmap_backbuffer_of_display_for_gl_projection;
   Camera3 camera3;
   ModelBin models;
   PlayerStats player_stats;

   LightracerMax(Framework &framework, Screens &screens, Display *display)
      : Screen(framework, screens, display)
      , sub_bitmap_backbuffer_of_display_for_gl_projection(nullptr)
      , camera3()
      , models()
      , player_stats()
   {}

   void initialize()
   {
      init_game();
      if (development) initialize_gl_render();
   }

   void primary_timer_func() override
   {
      motion.update(al_get_time());

      start_profile_timer("WHOLE UPDATE");
      al_clear_to_color(al_color_name("black"));

      if (logo_showing)
      {
         main_menu_timer_func(framework.current_event);
      }
      else
      {
         game_timer_func(framework.current_event);
         if (development) gl_render_func();
      }

      stop_profile_timer("WHOLE UPDATE");
      if (FLAG_draw_profile_graph) draw_profile_timer_graph(20, 20);
   }

   void initialize_gl_render()
   {
      camera3.stepout = vec3d(0, 0, 10);
		camera3.tilt = 0.6;

      models.set_path("data/models");
      ALLEGRO_BITMAP *main_target = al_get_backbuffer(display->al_display);
      sub_bitmap_backbuffer_of_display_for_gl_projection = al_create_sub_bitmap(main_target,
         0, 0, al_get_bitmap_width(main_target), al_get_bitmap_height(main_target));
   }

   void gl_render_func()
   {
      float racer_speed = racer->velocity_magnitude;
      good_camera->z += 100 + 30*(4.0-racer_speed);
      float camera_y = 550 - 65*racer_speed;  // higher numbers (400) mean flatter, more birds-eye perspective
      //float track_y_value = 50 + 50*(4.0-racer_speed) + (1.0-camera->zoom)*1000;

      camera3.spin = -camera->rotation + FULL_ROTATION/2;
      camera3.stepout = vec3d(0, 0, 10 + camera_y * 0.05);

      ALLEGRO_STATE previous_bitmap_state;
      al_store_state(&previous_bitmap_state, ALLEGRO_STATE_TARGET_BITMAP);
      al_set_target_bitmap(sub_bitmap_backbuffer_of_display_for_gl_projection);
      draw_gl_projection(camera3, racer, sub_bitmap_backbuffer_of_display_for_gl_projection, models, track);
      al_restore_state(&previous_bitmap_state);
   }

   void key_up_func() override
   {
      ::key_up_func(framework.current_event);
   }

   void key_down_func() override
   {
      ::key_down_func(framework, framework.current_event);
   }

   void key_char_func() override
   {
      ::key_char_func(framework.current_event);
   }
};


int main(int argc, char **argv)
{
   Screens screens;
   Framework framework(screens);
   framework.initialize();
   Display *display = framework.create_display(SCREEN_W, SCREEN_H);

   LightracerMax lightracer_max(framework, screens, display);
   lightracer_max.initialize();

   framework.run_loop();

   destroy_game();

   return 0;
}
