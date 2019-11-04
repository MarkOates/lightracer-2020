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



//int segment_where_player_died = 0;

vector<int> segment_where_player_died;



void start_track_begin_text()
{
   float start_time = al_get_time();
   float end_time = start_time + 4.0;
   motion.animate(&track_begin_notification_counter, 1.0, 0.0, start_time, end_time, interpolator::tripple_fast_out);
}


int num_of_segments_in_track = 4;



double OMG_DeltaTime = 1.0;

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



class CheapCamera
{
public:
   float depth;
   float z;
   float x, y, rotation;
   float zoom;
   ALLEGRO_TRANSFORM transform;

   CheapCamera()
   {
      depth = 512;
      z = 256;
      rotation = 0.0;
      zoom = 1.0;
   }

   void transform_on()
   {
      al_identity_transform(&transform);
      al_translate_transform(&transform, -x, -y);
      al_scale_transform(&transform, zoom, zoom);
      //al_rotate_transform(&transform, rotation+FULL_ROTATION/2);
      al_translate_transform(&transform, 400, 300);
      //al_build_transform(&transform, x, y, 1.0, 1.0, rotation);
      al_use_transform(&transform);
   }
};

CheapCamera *camera = nullptr;



class camera_class : public vec2d
{
private:

public:
   float z;

   //animatable<float> z;
   float depth;
   vec2d center_point;
   float home_z;

   bool keyboard_controls_on;

   camera_class()
   {
      //_z = 0.0;
      //z.set(&_z);

      depth = 128;  // the bigger this number, the less apparent the depth effect
      depth = 512;  // 512 is an accurate balance to where moving right 100px feels
                    // proportionally the same distance as moving forward 100px
      home_z = depth;
      z = home_z;
      center_point = vec2d(SCREEN_W/2, SCREEN_H/2);
      keyboard_controls_on = true;
   }

   ALLEGRO_COLOR foreground_color; 

   inline float get_scale(float object_z)
   {
      float distance_from_camera = z - object_z;
      if (distance_from_camera <= 0) return 0;
      if (depth <= 0) return 0;

      return depth / distance_from_camera;
   }

   inline vec2d get_displacement(float object_z)
   {
      float scale = get_scale(object_z);
      return vec2d(-x * scale + SCREEN_HW, -y * scale + SCREEN_HH);
   }

   //inline vec3d screen_coordinates_to_world(float 
   
   
   //inline vec2d get_coordinate_on_screen(float x, float y, float object_z)
   //{
      //return vec2d(x, y) + get_scale(object_z);
      //return ;
   //}
   

   vec2d get_xy_displacement()
   {
      return vec2d(-x + SCREEN_HW, -y + SCREEN_HH);
   }

   /*
   bool zoom_to(float position=512, float duration=3.0, float (*transition_curve)(float)=&interpolator::linear)
   {
      if (z.get_num_active_animations() != 0) return false;
      if (_z == position) return false;
      z.animate(_z, position, duration, transition_curve);
      return true;
   }
   */


   void draw_crosshair(float _x, float _y, float _z)
   {
      float scale = get_scale(_z);
      float width = 10;
      al_draw_line((_x-x)*scale - width*scale + center_point.x,
                  (_y-y)*scale + center_point.y,
                  (_x-x)*scale + width*scale + center_point.x,
                  (_y-y)*scale + center_point.y,
                  al_color_name("red"), 1.0*scale);
      al_draw_line((_x-x)*scale + center_point.x,
                  (_y-y)*scale - width*scale + center_point.y,
                  (_x-x)*scale + center_point.x,
                  (_y-y)*scale + width*scale + center_point.y,
                  al_color_name("red"), 1.0*scale);
   }

   vec2d get_point(float _x, float _y, float _z)
   {
      float scale = get_scale(_z);
      return vec2d((_x-x)*scale + center_point.x,  (_y-y)*scale + center_point.y);
   }

   void turn_on_controls()
   {
      keyboard_controls_on = true;
   }

   void turn_off_controls()
   {
      keyboard_controls_on = false;
   }

};


camera_class *good_camera = nullptr;


void zoom_way_out()
{
   motion.move_to(&camera->zoom, 0.05, 0.4);
}

void zoom_normal()
{
   motion.move_to(&camera->zoom, 1.0, 0.4);
}



#define LEFT_RAIL 1
#define RIGHT_RAIL 2

class LineSegmentInfo
{
public:
   vec2d start;
   vec2d end;
   vec2d from_start;
   vec2d perpendicular;
   vec2d normal;
   vec2d middle;
   float radius;
   float length;

   LineSegmentInfo() {}
   LineSegmentInfo(vec2d &start, vec2d &end)
   {
      this->start = start;
      this->end = end;
      from_start = end - start;
      middle = from_start/2 + start;
      length = distance(start, end);
      radius = length/2;
      perpendicular = vec2d(-from_start.y, from_start.x);
      normal = ~-perpendicular;
   }
};




class particle_effect;

void mark_particle_unused(particle_effect *p);

class particle_effect
{
public:
   static vector<particle_effect *> particle;
public:
   static int last_particle_index;
   particle_effect() : image(nullptr), in_use(false) {}
   static void reserve_particles(int num)
   {
      for (int i=0; i<(int)particle.size(); i++) delete particle[i];
      particle.clear();

      for (int i=0; i<num; i++)
         particle.push_back(new particle_effect());
   }
   static particle_effect *get_one()
   {
      return particle[++last_particle_index % particle.size()];
   }
   static void update_all()
   {
      for (int i=0; i<(int)particle.size(); i++) particle[i]->update();
   }

   bool update()
   {
      //if ((start_time + duration) > al_get_time()) return (in_use = false);
      position += velocity;
      return true;
   }
   float start_time;
   float duration;
   vec2d position;
   vec2d velocity;

   float position_z;
   float opacity;
   ALLEGRO_BITMAP *image;
   bool in_use;

   vec2d projected_position; // < ignore... it's for use the outside world
};
vector<particle_effect *> particle_effect::particle;
int particle_effect::last_particle_index = 0;

void mark_particle_unused(particle_effect *p)
{
   p->in_use = false;
}





enum { COLOR_TYPE_WHITE = 0,
   COLOR_TYPE_OFF,
   COLOR_TYPE_YELLOW,
   COLOR_TYPE_GREEN,
   COLOR_TYPE_BLUE,
   COLOR_TYPE_RED,
   COLOR_TYPE_RED_DEATH,
   COLOR_TYPE_MAX };


class TrackSegment
{
public:
   float w, h;

   float rotation;
   //vec2d displacement;

   std::vector<vec2d *> left_rail;
   std::vector<LineSegmentInfo *> left_rail_segment;
   float left_rail_length;

   std::vector<vec2d *> right_rail;
   std::vector<LineSegmentInfo *> right_rail_segment;
   float right_rail_length;


   vec2d *entrance_p1;
   vec2d *entrance_p2;
   LineSegmentInfo *entrance_segment_info;


   int color_type;

   int num_points()
   {
      return right_rail.size() + left_rail.size();
   }

   TrackSegment() : color_type(COLOR_TYPE_YELLOW), entrance_p1(nullptr), entrance_p2(nullptr), entrance_segment_info(nullptr)
    {}

   ~TrackSegment()
   {
      for (int i=0; i<(int)left_rail.size(); i++) delete left_rail[i];
      for (int i=0; i<(int)right_rail.size(); i++) delete right_rail[i];
      left_rail.clear();
      right_rail.clear();
      update_slope_info();
   }

   void add_point(int wall_num, float x, float y)
   {
      if (wall_num==LEFT_RAIL) left_rail.push_back(new vec2d(x, y));
      if (wall_num==RIGHT_RAIL) right_rail.push_back(new vec2d(x, y));
      //update_slope_info();
   }

   void move(float x, float y)
   {
      for (int i=0; i<(int)left_rail.size(); i++)
      {
         left_rail[i]->x += x;
         left_rail[i]->y += y;
      }
      for (int i=0; i<(int)right_rail.size(); i++)
      {
         right_rail[i]->x += x;
         right_rail[i]->y += y;
      }
      update_slope_info();
   }

   void rotate(float angle)
   {
      for (int i=0; i<(int)left_rail.size(); i++)
      {
         *left_rail[i] = rotate_point(*left_rail[i], angle);
      }
      for (int i=0; i<(int)right_rail.size(); i++)
      {
         *right_rail[i] = rotate_point(*right_rail[i], angle);
      }
      update_slope_info();
   }

   void draw_projected();

   void reverse()
   {
      // doesn't work (yet)
      update_slope_info();
   }

   void draw()
   {
      int line_thickness = 3;

      for (int i=1; i<(int)left_rail.size(); i++)
         al_draw_line(left_rail[i-1]->x, left_rail[i-1]->y, left_rail[i]->x, left_rail[i]->y, al_color_name("orange"), 2.0*line_thickness);

      for (int i=1; i<(int)left_rail.size(); i++)
         al_draw_filled_circle(left_rail[i-1]->x, left_rail[i-1]->y, 2.0*line_thickness/2, al_color_name("orange"));

      for (int i=1; i<(int)right_rail.size(); i++)
         al_draw_line(right_rail[i-1]->x, right_rail[i-1]->y, right_rail[i]->x, right_rail[i]->y, al_color_name("orange"), 2.0*line_thickness);

      for (int i=1; i<(int)right_rail.size(); i++)
         al_draw_filled_circle(right_rail[i-1]->x, right_rail[i-1]->y, 2.0*line_thickness/2, al_color_name("orange"));

      return;

      al_draw_circle(left_rail.front()->x, left_rail.front()->y, 12, al_color_name("green"), 3.0);
      al_draw_circle(left_rail.back()->x, left_rail.back()->y, 6, al_color_name("red"), 3.0);
   }

   void __HACK_reverse_order_of_points()
   {
      std::reverse(left_rail.begin(), left_rail.end());
      std::reverse(right_rail.begin(), right_rail.end());
      std::reverse(left_rail_segment.begin(), left_rail_segment.end());
      std::reverse(right_rail_segment.begin(), right_rail_segment.end());
      update_slope_info();
   }

   void soften_left_rail()
   {
      if (left_rail.size() <= 2) return;

      std::vector<vec2d *> *newpoints = new std::vector<vec2d *>;
      
      // break each segment into 4 seperate segments
      for (int i=1; i<(int)left_rail.size(); i++)
      {
         newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.0 + *left_rail[i-1]));
         newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.25 + *left_rail[i-1]));
         newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.5 + *left_rail[i-1]));
         newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.75 + *left_rail[i-1]));
      }
      newpoints->push_back(new vec2d(*left_rail[left_rail.size()-1]));

      
      // skip the first 2
      // erase every other point
      bool keep = true;
      for (int i=1; i<(int)newpoints->size(); i++)
      {
         if (!keep)
         {
            delete newpoints->at(i);
            newpoints->erase(newpoints->begin()+i);
            i--;
         }
         keep = !keep;
      }
      newpoints->push_back(new vec2d(*left_rail[left_rail.size()-1]));

      delete newpoints->at(1);
      newpoints->erase(newpoints->begin()+1);

      delete newpoints->at((int)newpoints->size()-2);
      newpoints->erase(newpoints->end()-2);


      for (int i=0; i<(int)left_rail.size(); i++) delete left_rail[i];
      left_rail.clear();
      left_rail = *newpoints;

      update_slope_info();
   }

   void soften_right_rail()
   {
      if (right_rail.size() <= 2) return;

      std::vector<vec2d *> *newpoints = new std::vector<vec2d *>;
      
      // break each segment into 4 seperate segments
      for (int i=1; i<(int)right_rail.size(); i++)
      {
         newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.0 + *right_rail[i-1]));
         newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.25 + *right_rail[i-1]));
         newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.5 + *right_rail[i-1]));
         newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.75 + *right_rail[i-1]));
      }
      newpoints->push_back(new vec2d(*right_rail[right_rail.size()-1]));

      
      // skip the first 2
      // erase every other point
      bool keep = true;
      for (int i=1; i<(int)newpoints->size(); i++)
      {
         if (!keep)
         {
            delete newpoints->at(i);
            newpoints->erase(newpoints->begin()+i);
            i--;
         }
         keep = !keep;
      }
      newpoints->push_back(new vec2d(*right_rail[right_rail.size()-1]));

      delete newpoints->at(1);
      newpoints->erase(newpoints->begin()+1);

      delete newpoints->at((int)newpoints->size()-2);
      newpoints->erase(newpoints->end()-2);


      for (int i=0; i<(int)right_rail.size(); i++) delete right_rail[i];
      right_rail.clear();
      right_rail = *newpoints;

      update_slope_info();
   }

   void update_slope_info()
   {
      // for the left //

      if (left_rail.size() <= 1) return;
      for (int i=0; i<(int)left_rail_segment.size(); i++) delete left_rail_segment[i];

      left_rail_segment.clear();
      left_rail_segment.push_back(new LineSegmentInfo());
      left_rail_length = 0;

      for (int i=1; i<(int)left_rail.size(); i++)
      {
         left_rail_segment.push_back(new LineSegmentInfo(*left_rail[i-1], *left_rail[i]));
         left_rail_length += left_rail_segment.back()->length;
      }

      // for the right //

      if (right_rail.size() <= 1) return;
      for (int i=0; i<(int)right_rail_segment.size(); i++) delete right_rail_segment[i];

      right_rail_segment.clear();
      right_rail_segment.push_back(new LineSegmentInfo());
      right_rail_length = 0;

      for (int i=1; i<(int)right_rail.size(); i++)
      {
         right_rail_segment.push_back(new LineSegmentInfo(*right_rail[i-1], *right_rail[i]));
         right_rail_length += right_rail_segment.back()->length;
      }

      // entrance

      //if (entrance_p1) delete entrance_p1;
      //if (entrance_p2) delete entrance_p2;

      //entrance_p1 = new vec2d(0, 1);
      //entrance_p2 = new vec2d(1, 0);
      //entrance_segment_info = new LineSegmentInfo(*entrance_p1, *entrance_p2);

      entrance_p1 = left_rail.back();
      entrance_p2 = right_rail.front();
      if (entrance_segment_info) delete entrance_segment_info;
      entrance_segment_info = new LineSegmentInfo(*entrance_p1, *entrance_p2);
   }
};



// track construction functions //


void fill_track_rail_points();

TrackSegment *create_track_segmentA()
{
   TrackSegment *segment = new TrackSegment();
   segment->add_point(LEFT_RAIL, 200, 500);
   segment->add_point(LEFT_RAIL, 200, 375);
   segment->add_point(LEFT_RAIL, 150, 275);
   segment->add_point(LEFT_RAIL, 150, 100);

   segment->add_point(RIGHT_RAIL, 250, 100);
   segment->add_point(RIGHT_RAIL, 250, 250);
   segment->add_point(RIGHT_RAIL, 300, 350);
   segment->add_point(RIGHT_RAIL, 300, 500);

   segment->move(-200, -500);

   return segment;
}


TrackSegment *create_track_segmentB()
{
   TrackSegment *segment = new TrackSegment();
   segment->add_point(LEFT_RAIL, 300, 500);
   segment->add_point(LEFT_RAIL, 300, 400);
   segment->add_point(LEFT_RAIL, 350, 350);
   segment->add_point(LEFT_RAIL, 350, 200);
   segment->add_point(LEFT_RAIL, 250, 100);
   segment->add_point(LEFT_RAIL, 0, 100);

   segment->add_point(RIGHT_RAIL, 0, 0);
   segment->add_point(RIGHT_RAIL, 450, 0);
   segment->add_point(RIGHT_RAIL, 450, 450);
   segment->add_point(RIGHT_RAIL, 400, 500);

   segment->move(-300, -500);

   return segment;
}


TrackSegment *create_track_segmentC()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 0, 500);
   segment->add_point(LEFT_RAIL, 0, 125);
   segment->add_point(LEFT_RAIL, 250, 50);

   segment->add_point(RIGHT_RAIL, 300, 500-363.396);
   segment->add_point(RIGHT_RAIL, 125, 225);
   segment->add_point(RIGHT_RAIL, 100, 500);

   segment->move(0, -500);

   return segment;
}


TrackSegment *create_track_segmentD()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 200, 500);
   segment->add_point(LEFT_RAIL, 200, 200);
   segment->add_point(LEFT_RAIL, 100, 100);
   segment->add_point(LEFT_RAIL, 0, 100);

   segment->add_point(RIGHT_RAIL, 0, 0);
   segment->add_point(RIGHT_RAIL, 150, 0);
   segment->add_point(RIGHT_RAIL, 300, 150);
   segment->add_point(RIGHT_RAIL, 300, 500);

   segment->move(-200, -500);

   return segment;
}



TrackSegment *create_track_segmentE()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 50, 1000);
   segment->add_point(LEFT_RAIL, 50, 650);
   segment->add_point(LEFT_RAIL, 0, 600);
   segment->add_point(LEFT_RAIL, 0, 200);
   segment->add_point(LEFT_RAIL, 50, 150);
   segment->add_point(LEFT_RAIL, 50, 0);

   segment->add_point(RIGHT_RAIL, 150, 0);
   segment->add_point(RIGHT_RAIL, 150, 150);
   segment->add_point(RIGHT_RAIL, 200, 200);
   segment->add_point(RIGHT_RAIL, 200, 600);
   segment->add_point(RIGHT_RAIL, 150, 650);
   segment->add_point(RIGHT_RAIL, 150, 1000);

   segment->move(-50, -1000);

   return segment;
}



TrackSegment *opening_gate()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 0, 500);
   segment->add_point(LEFT_RAIL, -50, 500);
   segment->add_point(LEFT_RAIL, -100, 425);
   segment->add_point(LEFT_RAIL, -100, 325);
   segment->add_point(LEFT_RAIL, -50, 225);
   segment->add_point(LEFT_RAIL, -50, 125);

   segment->add_point(RIGHT_RAIL, 50, 125);
   segment->add_point(RIGHT_RAIL, 50, 225);
   segment->add_point(RIGHT_RAIL, 100, 325);
   segment->add_point(RIGHT_RAIL, 100, 425);
   segment->add_point(RIGHT_RAIL, 50, 500);
   segment->add_point(RIGHT_RAIL, 0, 500);

   segment->move(50, 475);

   return segment;
}

class Racer;

class Track
{
public:

   static vector<int> __this_points_color__left_rail;
   static vector<int> __this_points_color__right_rail;
   static vector<int> __this_point_belongs_to_segment;

   static vector<vec2d> track_rail_light; // < for caching
   static vector<std::pair<bool, int> > rail_light_belongs_to; // < for caching
   static vector<vec2d> track_rail_light_projected; // < for caching

   static vector<std::pair<bool, int> > __left_color_light_belongs_to; // < for caching
   static vector<std::pair<bool, int> > __right_color_light_belongs_to; // < for caching

   static vector<float> car_distance_cache;

public:
   vec2d exit_p1;
   vec2d exit_p2;
   vec2d enter_p1;
   vec2d enter_p2;
   LineSegmentInfo *exit_segment_info;
   double start_time;

   std::vector<TrackSegment *> segment;
   ALLEGRO_VERTEX left_point[512];
   ALLEGRO_VERTEX right_point[512];


   Track() : exit_segment_info(nullptr) {}

   ~Track()
   {
      for (int i=0; i<(int)segment.size(); i++) delete segment[i];
      segment.clear();
      segment_where_player_died.clear();
   }
   void clear()
   {
      for (int i=0; i<(int)segment.size(); i++)
         delete segment[i];
      segment.clear();
      segment_where_player_died.clear();
   }
   void draw()
   {
      for (int i=0; i<(int)segment.size(); i++)
         segment[i]->draw();

      al_draw_line(exit_p1.x, exit_p1.y, exit_p2.x, exit_p2.y, al_color_name("dodgerblue"), 20.0);

      al_draw_line(enter_p1.x, enter_p1.y, enter_p2.x, enter_p2.y, al_color_name("lightgreen"), 20.0);
   }

   void draw_projected(CheapCamera *cam, float racer_facing_angle, float racer_x, float racer_y, float racer_speed);

   void update_exit_slope_info()
   {
      if (exit_segment_info) delete exit_segment_info;
      exit_segment_info = new LineSegmentInfo(exit_p1, exit_p2);
   }

   void __HACK_finalize_track()
   {
      for (int i=0; i<(int)segment.size(); i++)
         segment[i]->__HACK_reverse_order_of_points();

      update_exit_slope_info();
      fill_track_rail_points();
   }

   void make_segments_easy_where_player_died()
   {
      for (int i=0; i<(int)segment_where_player_died.size(); i++)
      {
         segment[segment_where_player_died[i]]->color_type = COLOR_TYPE_OFF;

         //std::cout << "EASY: " << segment_where_player_died[i] << std::endl;
      }
      fill_track_rail_points();
   }

   void create_gate()
   {
      TrackSegment *gate = opening_gate();
      segment.push_back(gate);
   }

   void append_segment(TrackSegment *ts)
   {
      if (segment.empty()) { segment.push_back(ts); return; }

      vec2d pipe_output_vector = *segment.back()->right_rail.front()- *segment.back()->left_rail.back();
      float distance = pipe_output_vector.get_magnitude();

      ts->rotate(pipe_output_vector.get_angle());

      /*
      std::cout << "x: " << pipe_output_vector.x << std::endl;
      std::cout << "y: " << pipe_output_vector.y << std::endl;
      std::cout << "angle: " << pipe_output_vector.get_angle() << std::endl;
      std::cout << "angle(rad): " << degrees_to_radians(pipe_output_vector.get_angle()) << std::endl;
      std::cout << "mag: " << distance << std::endl;
      std::cout << std::endl;
      */
      //std::cout << "angle(deg): " << radians_to_degrees(pipe_output_vector.get_angle()) << std::endl;
      ts->move(segment.back()->left_rail.back()->x, segment.back()->left_rail.back()->y);

      segment.push_back(ts);

      //ts->__HACK_reverse_order_of_points();

      // update the entrance / exit
      exit_p1 = *segment.back()->left_rail.back();
      exit_p2 = *segment.back()->right_rail.front();
      enter_p1 = *segment.at(1)->left_rail.front(); // the edge of the gate
      enter_p2 = *segment.at(1)->right_rail.back(); // the edge of the gate
      //fill_track_rail_points();
   }
};
vector<int> Track::__this_points_color__left_rail;
vector<int> Track::__this_points_color__right_rail;
vector<int> Track::__this_point_belongs_to_segment;
vector<vec2d> Track::track_rail_light; // < for caching
vector<vec2d> Track::track_rail_light_projected; // < for caching
vector<float> Track::car_distance_cache;
vector<std::pair<bool, int> > Track::rail_light_belongs_to;
vector<std::pair<bool, int> > Track::__left_color_light_belongs_to;
vector<std::pair<bool, int> > Track::__right_color_light_belongs_to;



void complete_track();
void mark_player_not_using_boost(void *);

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

   Racer()
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

   void complete_lap()
   {
      //num_laps++;
      //lap_time = al_get_time();
      lap_time.push_back(al_get_time());
   }

   void draw()
   {
      //vec2d direction_vector;
      al_draw_circle(position.x, position.y, 10, al_color_name("pink"), 2.0);
      al_draw_line(position.x, position.y, position.x+velocity.x*20, position.y+velocity.y*20, al_color_name("pink"), 2.0);
      al_draw_line(position.x, position.y, position.x+direction.x*20, position.y+direction.y*20, al_color_name("yellow"), 2.0);
   }

   void use_boost()
   {
      if (boosts == 0) { /* do nothing */ }
      if (using_boost) return;

      float start_time = al_get_time();
      float end_time = start_time + 2.0;
      motion.animate(&using_boost_counter, 1.0, 0.0, start_time, end_time, interpolator::double_fast_out, mark_player_not_using_boost);
      velocity_magnitude = 4.5;

      using_boost = true;
   }
};








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





inline vec2d get_dot_at_distance(int track_segment, float distance, bool left)
{
   // does left rail only
   if (left)
   {
      float length_traversed = 0;
      //for (int i=0; i<(int)track->segment.size(); i++)
      //{
         for (int rail=1; rail<(int)track->segment[track_segment]->left_rail.size(); rail++)
         {
            float segment_length = track->segment[track_segment]->left_rail_segment[rail]->length;
            //std::cout << "segment_length: " << segment_length << std::endl;

            if (distance <= (length_traversed + segment_length)
               && distance >= (length_traversed))
            {
               // dot is on this segment
               float dot_distance_along_segment = distance - length_traversed;
               //std::cout << "YAY";
               return track->segment[track_segment]->left_rail_segment[rail]->from_start.normalized() * dot_distance_along_segment
                  + track->segment[track_segment]->left_rail_segment[rail]->start;
            }

            length_traversed += segment_length;
         }
      //}

         return vec2d(-9999, -9999);
   }


      float length_traversed = 0;
      //for (int i=0; i<(int)track->segment.size(); i++)
      //{
         for (int rail=1; rail<(int)track->segment[track_segment]->right_rail.size(); rail++)
         {
            float segment_length = track->segment[track_segment]->right_rail_segment[rail]->length;
            //std::cout << "segment_length: " << segment_length << std::endl;

            if (distance <= (length_traversed + segment_length)
               && distance >= (length_traversed))
            {
               // dot is on this segment
               float dot_distance_along_segment = distance - length_traversed;
               //std::cout << "YAY";
               return track->segment[track_segment]->right_rail_segment[rail]->from_start.normalized() * dot_distance_along_segment
                  + track->segment[track_segment]->right_rail_segment[rail]->start;
            }

            length_traversed += segment_length;
         }

         return vec2d(-9999, -9999);
}

void fill_track_rail_points()
{
   vector<vec2d> &track_rail_point = Track::track_rail_light;
   //Track::track_rail_light.clear();
   track_rail_point.clear();
   Track::rail_light_belongs_to.clear();

   //Track::color_light_belongs_to.clear();





   // space the white lights:

   float total_length = 0;
   float dot_distance = 30;

   int segment_num = 0;
   for (int segment_num=0; segment_num<(int)track->segment.size(); segment_num++)
   {
      int rail = 1;
      for (; rail<(int)track->segment[segment_num]->left_rail.size(); rail++)
      {
         total_length += track->segment[segment_num]->left_rail_segment[rail]->length;
      }

      int d=0;
      do
      {
         track_rail_point.push_back(get_dot_at_distance(segment_num, d, true));
         Track::rail_light_belongs_to.push_back(std::pair<bool, int>(true, segment_num));
         d += dot_distance;
      }while (d<total_length);
   }


   // right rails

   total_length = 0;
   segment_num = 0;
   for (int segment_num=0; segment_num<(int)track->segment.size(); segment_num++)
   {
      int rail = 1;
      for (; rail<(int)track->segment[segment_num]->right_rail.size(); rail++)
      {
         total_length += track->segment[segment_num]->right_rail_segment[rail]->length;
      }

      int d=0;
      do
      {
         track_rail_point.push_back(get_dot_at_distance(segment_num, d, false));
         Track::rail_light_belongs_to.push_back(std::pair<bool, int>(false, segment_num));
         d += dot_distance;
      }while (d<total_length);
   }

   // match the "projected" cache and the car distance cache to the same size
   Track::track_rail_light_projected.clear();
   for (int i=0; i<(int)track_rail_point.size(); i++)
   {
      Track::track_rail_light_projected.push_back(vec2d(0,0));
      Track::car_distance_cache.push_back(0);
   }
}




void Track::draw_projected(CheapCamera *cam, float racer_direction_angle, float racer_x, float racer_y, float racer_speed)
{
   start_profile_timer("DP 1");
   //////////////////////////
   /* modify perspective angles with these!! */
   
   vec2d camera_target_offset = vec2d::polar_coords(racer_direction_angle, 50);

   vec2d racer_non_adjusted_pos(racer_x, racer_y);

   //racer_x += camera_target_offset.x;
   //racer_y += camera_target_offset.y;
   //racer_y -= 300;
   good_camera->z += 100 + 30*(4.0-racer_speed);

   float camera_y = 550 - 65*racer_speed;  // higher numbers (400) mean flatter, more birds-eye perspective
   float track_y_value = 50 + 50*(4.0-racer_speed) + (1.0-cam->zoom)*1000;
   float multiplier = 0.15;

   //////////////////////////


   ALLEGRO_TRANSFORM transform;
   al_identity_transform(&transform);
   //al_scale_transform(&transform, camera->zoom, camera->zoom);
   al_translate_transform(&transform, 0, -camera_y);
   al_use_transform(&transform);

   //for (int ct=0; ct<6; ct++)
   //{
   // multiplier += 0.1;


   //
   vector<int> &this_points_color__left_rail = Track::__this_points_color__left_rail;
   vector<int> &this_points_color__right_rail = Track::__this_points_color__right_rail;

   vector<int> this_left_point_belongs_to_segment;;
   vector<int> this_right_point_belongs_to_segment;;

   this_points_color__left_rail.clear();
   this_points_color__right_rail.clear();

   
   Track::__left_color_light_belongs_to.clear();
   Track::__right_color_light_belongs_to.clear();


      int left_index_ct = 0;
      int right_index_ct = 0;
      float camera_rotation = -camera->rotation + FULL_ROTATION/2;

      for (int seg=0; seg<(int)segment.size(); seg++)
      {
         for (int i=segment[seg]->left_rail.size()-1; i>=1; i--)
         {
            if (i >= 512) goto nomore;
            vec2d point = *segment[seg]->left_rail[i];

            rotate_point(&point, vec2d(racer_x, racer_y), radians_to_degrees(camera_rotation)); 

            Track::__left_color_light_belongs_to.push_back(std::pair<bool, int>(true, seg));

            left_point[left_index_ct].x = point.x;
            left_point[left_index_ct].y = track_y_value;
            left_point[left_index_ct].z = point.y;
            //left_point[left_index_ct].color = al_color_name("white");

            this_points_color__left_rail.push_back(segment[seg]->color_type);
//          this_left_point_belongs_to_segment.push_back(seg);

            //project_point(left_point[left_index_ct], cam, multiplier);

            left_index_ct++;
         }

         for (int i=0; i<(int)segment[seg]->right_rail.size()-1; i++)
         {
            if (i >= 512) goto nomore;
            vec2d point = *segment[seg]->right_rail[i];

            rotate_point(&point, vec2d(racer_x, racer_y), radians_to_degrees(camera_rotation)); 

            Track::__right_color_light_belongs_to.push_back(std::pair<bool, int>(false, seg));

            right_point[right_index_ct].x = point.x;
            right_point[right_index_ct].y = track_y_value;
            right_point[right_index_ct].z = point.y;
            //right_point[right_index_ct].color = al_color_name("blue");

            this_points_color__right_rail.push_back(segment[seg]->color_type);
//          this_left_point_belongs_to_segment.push_back(seg);
            //project_point(right_point[right_index_ct], cam, multiplier);

            right_index_ct++;
         }
      }

      //std::cout << index_ct << ".";

   nomore:
   stop_profile_timer("DP 1");

      //al_draw_prim(left_point, NULL, NULL, 0, left_index_ct, ALLEGRO_PRIM_LINE_STRIP);
      //al_draw_prim(right_point, NULL, NULL, 0, right_index_ct, ALLEGRO_PRIM_LINE_STRIP);

   start_profile_timer("DP 2");
   ALLEGRO_BITMAP *yellow_ball = bitmaps.auto_get("yellow_ball.png");;


   float _align_x = 0.5;
   float _align_y = 0.5;
   float _scale_x = 0.5;
   float _scale_y = 0.5;
   float _rotation = 0.0;
   int flags = NULL;




   //////////////////////////


      //vec2d rect_tl = racer_non_adjusted_pos + vec2d(-20, -10);
      //vec2d rect_br = racer_non_adjusted_pos + vec2d(20, 10);

   // left tail light




   //al_draw_triangle(left_light_pos.x, left_light_pos.y, left_light_pos.x,
   // right_light_pos.x, right_light_pos.x, right_light_pos.y, al_color_name("white"), 1.0);

   {

      rotate_point(&racer_non_adjusted_pos, vec2d(racer_x, racer_y), radians_to_degrees(camera_rotation)); 

      float racer_to_draw_x = racer_non_adjusted_pos.x;
      float racer_to_draw_y = track_y_value;
      float racer_to_draw_z = racer_non_adjusted_pos.y;


      float racer_depth_scale = good_camera->get_scale(racer_to_draw_z);
      al_draw_scaled_rotated_bitmap(bitmaps.auto_get("racer3.png"),
                              al_get_bitmap_width(bitmaps.auto_get("racer3.png"))*_align_x,
                              al_get_bitmap_height(bitmaps.auto_get("racer3.png"))*_align_y,
                              (racer_to_draw_x - good_camera->x)*racer_depth_scale + good_camera->center_point.x,
                              (racer_to_draw_y - good_camera->y)*racer_depth_scale + good_camera->center_point.y,
                              _scale_x*racer_depth_scale*0.8,
                              _scale_y*racer_depth_scale*0.8,
                              _rotation,
                              flags);

      float engine_alpha = racer->velocity_magnitude;

      al_draw_tinted_scaled_rotated_bitmap(bitmaps.auto_get("there_are_3_lights.png"),
         al_map_rgba_f(engine_alpha, engine_alpha, engine_alpha, engine_alpha),
                              al_get_bitmap_width(bitmaps.auto_get("there_are_3_lights.png"))*_align_x,
                              al_get_bitmap_height(bitmaps.auto_get("there_are_3_lights.png"))*_align_y,
                              (racer_to_draw_x - good_camera->x)*racer_depth_scale + good_camera->center_point.x,
                              (racer_to_draw_y - good_camera->y)*racer_depth_scale + good_camera->center_point.y,
                              _scale_x*racer_depth_scale*0.8,
                              _scale_y*racer_depth_scale*0.8,
                              _rotation,
                              flags);

      if ((fmod(engine_alpha, 1.0f) < 0.2) && (racer->velocity_magnitude > 3))
      {

      al_draw_tinted_scaled_rotated_bitmap(bitmaps.auto_get("3_light_glow.png"),
         al_map_rgba_f(1.0, 1.0, 1.0, 1.0),
                              al_get_bitmap_width(bitmaps.auto_get("3_light_glow.png"))*_align_x,
                              al_get_bitmap_height(bitmaps.auto_get("3_light_glow.png"))*_align_y,
                              (racer_to_draw_x - good_camera->x)*racer_depth_scale + good_camera->center_point.x,
                              (racer_to_draw_y - good_camera->y)*racer_depth_scale + good_camera->center_point.y,
                              _scale_x*racer_depth_scale*0.8,
                              _scale_y*racer_depth_scale*0.8,
                              _rotation,
                              flags);
      }

   }


   //////////////////////////




   yellow_ball = bitmaps.auto_get("upbeam_light.png");


   //// DRAW PARTICLES

   //goto no_particles_yo;

   bool draw_particles = false;

   if (draw_particles)
   {

      int particle_draw_count = 0;
      for (int i=0; i<(int)particle_effect::particle.size(); i++)
      {
         particle_effect *particle = particle_effect::particle[i];
         if (particle->in_use && particle->image)
         {
            particle->projected_position = particle->position;
            rotate_point(&particle->projected_position, vec2d(racer_x, racer_y), radians_to_degrees(camera_rotation)); 

            float point_x = particle->projected_position.x;
            float point_y = track_y_value;
            float point_z = particle->projected_position.y;

            if (point_z < 0) continue;

            float depth_scale = good_camera->get_scale(point_z);
            al_draw_scaled_rotated_bitmap(particle->image,
                                   al_get_bitmap_width(particle->image)*_align_x,
                                   al_get_bitmap_height(particle->image)*_align_y,
                                   (point_x - good_camera->x)*depth_scale + good_camera->center_point.x,
                                   (point_y - good_camera->y)*depth_scale + good_camera->center_point.y,
                                   _scale_x*depth_scale,
                                   _scale_y*depth_scale,
                                   _rotation,
                                   flags);

            //particle_draw_count++;
         }
      }
   }

   //no_particles_yo:
   stop_profile_timer("DP 2");

   //std::cout << "drawing particles : " << particle_draw_count << std::endl;





   start_profile_timer("DP drawing");

   //// DRAW THE SMALL LIGHTS

   start_profile_timer("DP rail lights");

   yellow_ball = bitmaps.auto_get("upbeam_light.png");
   ALLEGRO_BITMAP *rail_light_off = bitmaps.auto_get("light_off.png");

   //fill_track_rail_points();

   //std::cout << track_rail_point.size() << std::endl;

   float distance_from_closest_light = 99999;
   float distance_from_light = 0;

   start_profile_timer("DP int vector copy");
   //Track::track_rail_light_projected = Track::track_rail_light;

   static int cache_start_num = 0; // basically, only update every 10th point, cycling each call
   cache_start_num++;
   if (cache_start_num >= 10) cache_start_num = 0; 

   //int cache_ct = 0;
   Track::track_rail_light_projected = Track::track_rail_light;
   for (int i=0; i<(int)Track::track_rail_light.size(); i++)
   {
      if (((i+cache_start_num)%10) == 0)
         Track::car_distance_cache[i] = distance(racer_x, racer_y, track_rail_light[i].x, track_rail_light[i].y);
      //cache_ct++;
   }

   vector<vec2d> &track_rail_point = Track::track_rail_light_projected;
   stop_profile_timer("DP int vector copy");




   //goto no_rail_lights;
   vec2d racer_vector(racer_x, racer_y);

   float bitmap_h_width_cache = al_get_bitmap_width(yellow_ball)*_align_x;
   float bitmap_h_height_cache = al_get_bitmap_height(yellow_ball)*_align_y;

   start_profile_timer("DP rail drawing");
   int draw_tinted = 0;
   ALLEGRO_BITMAP *image_to_use = NULL;
   for (int i=0; i<(int)track_rail_point.size(); i++)
   {
      if (Track::car_distance_cache[i] > 1000) continue;
      //if (Track::car_distance_cache[i] > 1000) draw_tinted = 

      image_to_use = yellow_ball;
      float height_multplier = 1.0;
      if (Track::car_distance_cache[i] > 800) height_multplier = 20.0 - 20.0 * ((1000-Track::car_distance_cache[i])/200) + 1.0;


      // swap out if the rail light is off:
      if (!((Track::rail_light_belongs_to[i].second == index_of_last_track_segment_that_collides)
        || (Track::rail_light_belongs_to[i].second == index_of_last_track_segment_that_collides+1)))
       
      {
         image_to_use = rail_light_off;
         height_multplier = 1.0;
      }


      rotate_point(&track_rail_point[i], racer_vector, radians_to_degrees(camera_rotation)); 
      float &point_x = track_rail_point[i].x;
      float &point_y = track_y_value;
      float &point_z = track_rail_point[i].y;

      //distance_from_light = distance(point_x, point_z, racer_x, racer_y);
      //if (distance_from_light < distance_from_closest_light) distance_from_closest_light = distance_from_light;

      float depth_scale = good_camera->get_scale(point_z);
         al_draw_scaled_rotated_bitmap(image_to_use,
                                bitmap_h_width_cache,
                                al_get_bitmap_height(image_to_use)*_align_y,
                                (point_x - good_camera->x)*depth_scale + good_camera->center_point.x,
                                (point_y - good_camera->y)*depth_scale + good_camera->center_point.y,
                                _scale_x*depth_scale,
                                _scale_y*depth_scale*depth_scale*height_multplier,
                                _rotation,
                                flags);

      //std::cout << "point( " << track_rail_point[i].x << ", \t\t" << track_rail_point[i].y <<  std::endl;
      //al_draw_bitmap(yellow_ball, track_rail_point[i].x, track_rail_point[i].y, NULL);
   }
   stop_profile_timer("DP rail drawing");
   //no_rail_lights:
   /*
   static float closest_lamp_delay = 10;
   if (distance_from_closest_light < 40 && racer_speed > 1.0)
   {
      closest_lamp_delay = 10;
      sound("close_lamp").vol(0.05*(40/distance_from_closest_light+1)).play();
   }
   closest_lamp_delay-=3;
   */
   //

   stop_profile_timer("DP rail lights");



   ////// DRAW THE POLE LIGHTS

   yellow_ball = bitmaps.auto_get("pole.png");
   static int strobe = 0;
   strobe++;
   if (strobe > 6) { strobe = 0;}

   // draw axis points

   //for (int i=0; i<=left_index_ct; i++)
   

   for (int i=left_index_ct-1; i>0; i--)
   {
      bool strobes = false;
      int color_type = this_points_color__left_rail[i];



      // if is within 3 segments of the 
      //if (!(Track::__left_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides-1)
      // || Track::__left_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides)
      // || Track::__left_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides+1)
      // )) continue;
      //if (Track::__left_color_light_belongs_to[i].second != (index_of_last_track_segment_that_collides)) continue;


      switch(color_type)
      {
      case COLOR_TYPE_WHITE:
         //return;
         if (strobe == 1) yellow_ball = bitmaps.auto_get("white_light.png");
         if (strobe == 2) yellow_ball = bitmaps.auto_get("pole_red.png");
         if (strobe == 3) yellow_ball = bitmaps.auto_get("white_light.png");
         if (strobe == 4) yellow_ball = bitmaps.auto_get("pole_blue.png");
         if (strobe == 5) yellow_ball = bitmaps.auto_get("white_light.png");
         if (strobe == 6) yellow_ball = bitmaps.auto_get("pole_green.png");
         break;
      case COLOR_TYPE_RED_DEATH:
         strobes = true;
         yellow_ball = bitmaps.auto_get("pole_red.png");
         break;
      case COLOR_TYPE_YELLOW:
         yellow_ball = bitmaps.auto_get("pole.png");
         break;
      case COLOR_TYPE_GREEN:
         yellow_ball = bitmaps.auto_get("pole_green.png");
         break;
      case COLOR_TYPE_BLUE:
         yellow_ball = bitmaps.auto_get("pole_blue.png");
         break;
      case COLOR_TYPE_OFF:
         yellow_ball = bitmaps.auto_get("light_off2.png");
         break;
      case COLOR_TYPE_RED:
         yellow_ball = bitmaps.auto_get("pole_red.png");
         break;
      }

      if (strobes && (strobe > 3)) yellow_ball = bitmaps.auto_get("pole_black.png");


      if (!(Track::__left_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides-1)
         || Track::__left_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides)
         || Track::__left_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides+1)
         )) yellow_ball = bitmaps.auto_get("light_off2.png");


      float depth_scale = good_camera->get_scale(left_point[i].z);
         al_draw_scaled_rotated_bitmap(yellow_ball,
                                al_get_bitmap_width(yellow_ball)*_align_x,
                                al_get_bitmap_height(yellow_ball)*0.6,
                                (left_point[i].x - good_camera->x)*depth_scale + good_camera->center_point.x,
                                (left_point[i].y - good_camera->y)*depth_scale + good_camera->center_point.y,
                                _scale_x*depth_scale,
                                _scale_y*depth_scale,
                                _rotation,
                                flags);
   }

   // draw axis points

   for (int i=0; i<right_index_ct; i++)
   {
      bool strobes = false;
      int color_type = this_points_color__right_rail[i];


      // if is within 3 segments of the 
      //if (Track::__right_color_light_belongs_to[i].second != (index_of_last_track_segment_that_collides)) continue;


      switch(color_type)
      {
      case COLOR_TYPE_WHITE:
         //return;
         if (strobe == 1) yellow_ball = bitmaps.auto_get("pole.png");
         if (strobe == 2) yellow_ball = bitmaps.auto_get("pole_red.png");
         if (strobe == 3) yellow_ball = bitmaps.auto_get("pole.png");
         if (strobe == 4) yellow_ball = bitmaps.auto_get("pole_blue.png");
         if (strobe == 5) yellow_ball = bitmaps.auto_get("white_light.png");
         if (strobe == 6) yellow_ball = bitmaps.auto_get("pole_green.png");
         break;
      case COLOR_TYPE_RED_DEATH:
         yellow_ball = bitmaps.auto_get("pole_red.png");
         strobes = true;
         break;
      case COLOR_TYPE_OFF:
         yellow_ball = bitmaps.auto_get("light_off2.png");
         break;
      case COLOR_TYPE_YELLOW:
         yellow_ball = bitmaps.auto_get("pole.png");
         break;
      case COLOR_TYPE_GREEN:
         yellow_ball = bitmaps.auto_get("pole_green.png");
         break;
      case COLOR_TYPE_BLUE:
         yellow_ball = bitmaps.auto_get("pole_blue.png");
         break;
      case COLOR_TYPE_RED:
         yellow_ball = bitmaps.auto_get("pole_red.png");
         break;
      }

      if (strobes && (strobe > 3)) yellow_ball = bitmaps.auto_get("pole_black.png");



      if (!(Track::__right_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides-1)
         || Track::__right_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides)
         || Track::__right_color_light_belongs_to[i].second == (index_of_last_track_segment_that_collides+1)
         )) yellow_ball = bitmaps.auto_get("light_off2.png");


      float depth_scale = good_camera->get_scale(right_point[i].z);
         al_draw_scaled_rotated_bitmap(yellow_ball,
                                al_get_bitmap_width(yellow_ball)*_align_x,
                                al_get_bitmap_height(yellow_ball)*0.6,
                                (right_point[i].x - good_camera->x)*depth_scale + good_camera->center_point.x,
                                (right_point[i].y - good_camera->y)*depth_scale + good_camera->center_point.y,
                                _scale_x*depth_scale,
                                _scale_y*depth_scale,
                                _rotation,
                                flags);
      
//    al_draw_bitmap(yellow_ball, left_point[i].x, left_point[i].y, NULL);
   }

   //}
   //vec2d car_position(racer_x, racer_y);
   //project_point(car_position, cam);
   //al_draw_circle(car_position.x, car_position.y, 8, al_color_name("dodgerblue"), 3.0);




   //ALLEGRO_TRANSFORM transform2;
   //al_identity_transform(&transform2);
   //al_scale_transform(&transform2, camera->zoom, camera->zoom);
   //al_translate_transform(&transform, 0, -600);
   //al_rotate_transform(&transform2, camera->rotation+FULL_ROTATION/2);
   //al_translate_transform(&transform2, 400, 300);
   //al_use_transform(&transform2);
   stop_profile_timer("DP drawing");


   // draw evenly distributed dots along border


}
//void draw_projected(TrackSegment *segment)
//{
// for (int i
//}



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


void draw_health_bar_OLD()
{
   int health_bar_height = 10;

   std::string lives_string = "Lives: " + tostring(num_lives);
   al_draw_text(fonts["venus_rising_rg.ttf 26"], al_color_name("white"), 20, SCREEN_H-60, 0, lives_string.c_str());

   float health_percentage = racer->health/racer->max_health;
   ALLEGRO_COLOR health_bar_color;
   if (health_percentage > 0.9) health_bar_color = al_color_name("yellow");
   else if (health_percentage > 0.6) health_bar_color = al_color_name("yellow");
   else if (health_percentage > 0.3) health_bar_color = al_color_name("orange");
   else health_bar_color = al_color_name("red");


   al_draw_filled_rectangle(20, SCREEN_H-20, 220, SCREEN_H-(20 + health_bar_height), al_color_name("black"));
   al_draw_filled_rectangle(20, SCREEN_H-20, 20+200*(health_percentage), SCREEN_H-(20 + health_bar_height), health_bar_color);
}


void draw_health_bar(int text_y, int horizontal_screen_padding)
{
   int health_bar_height = 10;
   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];

   std::string lives_string = "Lives: " + tostring(num_lives);
   al_draw_text(font, al_color_name("white"), horizontal_screen_padding, text_y, 0, lives_string.c_str());

   float health_percentage = racer->health/racer->max_health;

   ALLEGRO_COLOR health_bar_color;
   if (health_percentage > 0.9) health_bar_color = al_color_name("dodgerblue");
   else if (health_percentage > 0.6) health_bar_color = al_color_name("violet");
   else if (health_percentage > 0.3) health_bar_color = al_color_name("orange");
   else health_bar_color = al_color_name("red");

   int lives_text_length = 170;
   int bar_length = 220;
   int bar_y = text_y + al_get_font_ascent(font) / 2 + 2;
   int bar_x = horizontal_screen_padding + lives_text_length + 4;

   al_draw_filled_rectangle(
         bar_x,
         bar_y - health_bar_height * 0.5,
         bar_x + bar_length,
         bar_y + health_bar_height * 0.5,
         al_color_name("darkblue"));

   al_draw_filled_rectangle(
         bar_x,
         bar_y - health_bar_height * 0.5,
         bar_x + bar_length * health_percentage,
         bar_y + health_bar_height * 0.5,
         al_color_name("lightblue"));
}


void draw_stopwatch(int text_y, int horizontal_screen_padding)
{
   std::string ellapsed_time_str = TimerFormatter(stopwatch.get_elappsed_time_msec()).format();
   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");

   //float text_width = al_get_text_width(font, ellapsed_time_str.c_str());
   std::string the_char = " ";
   int letter_spacing = 12 * 2;
   int letters_traversed = 0;
   for (auto &c : ellapsed_time_str)
   {
      the_char[0] = c;
      al_draw_text(font, color, SCREEN_W - 300 + (letters_traversed * letter_spacing), text_y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
}   


void draw_num_segments(int text_y, int num_segments)
{
   std::stringstream ss;
   ss << num_segments << " SEG";

   std::string string_to_write = ss.str();

   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, SCREEN_W - 300 - 300, text_y, ALLEGRO_ALIGN_CENTER, string_to_write.c_str());
}


void draw_num_laps(int text_y, int current_lap_num, int total_num_laps)
{
   std::string string_to_write = "";

   if (current_lap_num < total_num_laps)
   {
      std::stringstream ss;
      ss << current_lap_num << " / " << total_num_laps;
      string_to_write = ss.str();
   }
   else if (current_lap_num == total_num_laps)
   {
      string_to_write = "FIN";
   }
   else
   {
      string_to_write = "";
   }

   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, SCREEN_W - 300 - 300 - 200, text_y, ALLEGRO_ALIGN_CENTER, string_to_write.c_str());
}


void draw_hud()
{
   ALLEGRO_TRANSFORM transform;
   al_identity_transform(&transform);
   al_use_transform(&transform);

   
   double lap_so_far = 0;
   

  int text_y = 40;
  int horizontal_screen_padding = 100;

   draw_num_segments(text_y, num_of_segments_in_track);

   draw_num_laps(text_y, (int)racer->lap_time.size()+1, num_laps_to_win);
   draw_stopwatch(text_y, horizontal_screen_padding);

   draw_health_bar(text_y, horizontal_screen_padding);
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



void update_racer_and_track(Racer *r, Track *track) // includes masking
{

   static int every_other = 0;
   every_other++;

   //if ((every_other%2) != 0) return;

   static float turn_direction_velocity = 0;
   //if (r->throttle_on) r->velocity_magnitude += 0.065; // good


   if (r->throttle_on) r->velocity_magnitude += 0.085;
   if (r->break_on) r->velocity_magnitude *= 0.9;
   if (r->turning_right) turn_direction_velocity -= 0.014; // good
   if (r->turning_left) turn_direction_velocity += 0.014; // good

   turn_direction_velocity = (turn_direction_velocity * 0.8); // good

   r->direction_angle += turn_direction_velocity * OMG_DeltaTime;

   r->velocity_magnitude *= (1.0-abs(turn_direction_velocity*0.05));// = (turn_direction_velocity * 0.8); // good
   
   // friction
   //r->velocity_magnitude *= 0.98;
   r->velocity_magnitude *= 0.98; // good



   r->direction = vec2d(sin(r->direction_angle), cos(r->direction_angle));
   r->velocity = (r->velocity*0.6 + r->direction*0.4).normalized() * r->velocity_magnitude;



   Racer &player = *r;




   int __HACK_entrance_already_collided = 0;



   //if (!index_of_last_terrain_that_collides) last_terrain_that_collides = track->segment[0];



   //player.velocity_y += GRAVITY;
   //player.update_walking_velocity();
   //if (!player.moving_left && !player.moving_right) player.velocity_x *= 0.5;
   //player.on_ground = false;

   vec2d start_point = vec2d(player.position.x, player.position.y);
   vec2d end_point = start_point + vec2d(player.velocity.x, player.velocity.y) * player.velocity_magnitude * OMG_DeltaTime;
   

   vec2d player_pos = start_point;
   vec2d player_vel = end_point - start_point;
   vec2d player_end_pos = end_point;


   //player.position = end_point;
   //return;



   float time_left_in_timestep = 1.0;
   int segment_that_collides = 0;
   //Terrain *terrain_that_collides = NULL;
   TrackSegment *terrain_that_collides = nullptr;
   bool collides_on_left_terrain = false;
   vec2d point_of_intersection;

   int num_steps = 0;

   bool collides_through_exit = false;
   
   bool collides_through_entrance = false;
   int entrance_that_collides = 0;

   //if(debug) draw_crosshair(player_pos, al_color_name("green"));

   int terrain_segment_where_player_collides = 0;

   while(time_left_in_timestep > 0)
   {
      entrance_that_collides = 0;
      //
      // find the soonest intersection time
      //

      float collision_time = 1.0;

      vec2d __start = player_pos;
      vec2d __end = player_pos+player_vel*time_left_in_timestep;
      LineSegmentInfo motion_segment(__start, __end);

      vec2d &E = motion_segment.from_start;//*terrain.point[i] - *terrain.point[i-1];
      vec2d &P1 = motion_segment.perpendicular;//Vec2d(-F.y, F.x);
      terrain_that_collides = nullptr;
      collides_through_exit = false;
      collides_through_entrance = false;

      //for (int t=0; t<(int)map.terrain.size(); t++)
      int track_segment_start = std::max(index_of_last_track_segment_that_collides-2, 0);
      int track_segment_end = std::min(index_of_last_track_segment_that_collides+2, (int)track->segment.size()-1);

      //std::cout << "collision start end: " << track_segment_start << " -- " << track_segment_end << std::endl;

      for (int t=track_segment_start; t<(int)(track_segment_end+1); t++)
      {
         // just the left rails first

         //Terrain *terrain = map.terrain[t];
         TrackSegment *terrain = track->segment[t];

         
         //for (int i=1; i<(int)terrain->point.size(); i++)
         for (int i=1; i<(int)terrain->left_rail.size(); i++)
         {
            vec2d &F = terrain->left_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
            vec2d &P2 = terrain->left_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

            float h = ((motion_segment.start - terrain->left_rail_segment[i]->start) * P1) / (F * P1);
            float g = ((terrain->left_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

            if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
            {
               // there is a collision in this time-step
               // if it's less than another collision that occured, set the values
               if (g < collision_time)
               {
                  collision_time = g; // it occurs at g of this step
                  segment_that_collides = i;
                  point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                  terrain_that_collides = terrain;
                  //index_of_last_track_segment_that_collides = t;
                  terrain_segment_where_player_collides = t;
                  collides_on_left_terrain = true; // <---!!!important
                  collides_through_exit = false;
                  collides_through_entrance = false;
               }
            }
         }

         // check the exit

         vec2d &F = track->exit_segment_info->from_start;
         vec2d &P2 = track->exit_segment_info->perpendicular;//Vec2d(-F.y, F.x);

         float h = ((motion_segment.start - track->exit_segment_info->start) * P1) / (F * P1);
         float g = ((track->exit_segment_info->start - motion_segment.start) * P2) / (E * P2);

         if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
         {
            // there is a collision in this time-step
            // if it's less than another collision that occured, set the values
            if (g < collision_time)
            {
               collides_through_exit = true;
               collision_time = g; // it occurs at g of this step
               //segment_that_collides = i;
               point_of_intersection = g*motion_segment.from_start + motion_segment.start;
               terrain_that_collides = nullptr;
               collides_through_entrance = false;
               //collides_on_left_terrain = true; // <---!!!important
            }
         }

         // check the entrance to the next segment

         {
            vec2d &F = track->segment[t]->entrance_segment_info->from_start;
            vec2d &P2 = track->segment[t]->entrance_segment_info->perpendicular;//Vec2d(-F.y, F.x);

            float h = ((motion_segment.start - track->segment[t]->entrance_segment_info->start) * P1) / (F * P1);
            float g = ((track->segment[t]->entrance_segment_info->start - motion_segment.start) * P2) / (E * P2);

            if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
            {
               // there is a collision in this time-step
               // if it's less than another collision that occured, set the values
               if (g < collision_time)
               {
                  collides_through_entrance = true;
                  entrance_that_collides = t;

                  collides_through_exit = false;
                  collision_time = g; // it occurs at g of this step
                  //segment_that_collides = i;
                  
                  point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                  terrain_that_collides = nullptr;
                  //collides_on_left_terrain = true; // <---!!!important
               }
            }
         }

         // now the right rails

         for (int i=1; i<(int)terrain->right_rail.size(); i++)
         {
            vec2d &F = terrain->right_rail_segment[i]->from_start;//*terrain.point[i] - *terrain.point[i-1];
            vec2d &P2 = terrain->right_rail_segment[i]->perpendicular;//Vec2d(-F.y, F.x);

            float h = ((motion_segment.start - terrain->right_rail_segment[i]->start) * P1) / (F * P1);
            float g = ((terrain->right_rail_segment[i]->start - motion_segment.start) * P2) / (E * P2);

            if (h >= 0 && h <= 1 && g >= 0 && g <= 1)
            {
               // there is a collision in this time-step
               // if it's less than another collision that occured, set the values
               if (g < collision_time)
               {
                  collision_time = g; // it occurs at g of this step
                  segment_that_collides = i;
                  terrain_segment_where_player_collides = t;
                  //index_of_last_track_segment_that_collides = t;
                  point_of_intersection = g*motion_segment.from_start + motion_segment.start;
                  terrain_that_collides = terrain;
                  collides_on_left_terrain = false; // <---!!!important
                  collides_through_exit = false;
                  collides_through_entrance = false;
               }
            }
         }
      }

      //
      // move everything by that timestep
      // adjust velocities of colliding objects
      // decrement timestep by amount within timestep
      //

      player_pos += player_vel*time_left_in_timestep*collision_time;

      if (collides_through_entrance && (__HACK_entrance_already_collided==0))
      {
         //std::cout << "OUT!";
         __HACK_entrance_already_collided++;
         //__HACK_entrance_already_collided %= 10;
         player_pos += player_vel * 0.01; // < this is a solution that causes a *rare* *rare* bug where the player
                                  // can escape the track by colliding precicely with the segment entrance and wall
                                  // 

         //if (entrance_that_collides < index_of_last_track_segment_that_collides)
         // index_of_last_track_segment_that_collides--;
         //if (entrance_that_collides > index_of_last_track_segment_that_collides)
            index_of_last_track_segment_that_collides = entrance_that_collides;
            //sound("click").vol(0.5).play();
            al_stop_sample_instance(passthough_sample_instance);
            al_play_sample_instance(passthough_sample_instance);
      }

      if (collides_through_exit)
      {
         index_of_last_track_segment_that_collides = 0;

         //sound("leaving").play();
         //std::cout << std::endl << "warp!!" << std::endl << std::endl;
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

         //collision_time*motion_segment.from_start
         //player_vel = 
      }
      else if (terrain_that_collides != nullptr) 
      {
            //std::cout << "c";
            // displace along colliding normal (offset padding)
            if (collides_on_left_terrain) player_pos += terrain_that_collides->left_rail_segment[segment_that_collides]->normal * 0.1;
            if (!collides_on_left_terrain) player_pos += terrain_that_collides->right_rail_segment[segment_that_collides]->normal * 0.1;

            // this will bounce the object *while* preserving the x velocity
            //player_vel.x = player_vel.x;
            //player_vel.y = -player_vel.y;

            // this will reflect along the terrain segment (as if a ball bouncing, or light reflecting)
            if (collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->left_rail_segment[segment_that_collides]->normal);
            if (!collides_on_left_terrain) player_vel = reflect(player_vel, terrain_that_collides->right_rail_segment[segment_that_collides]->normal);



         switch(terrain_that_collides->color_type)
         {
         case COLOR_TYPE_RED:
            //sound("wall_deflect").vol(1.0).speed(random_float(0.9, 1.1)).play();
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
            //sound("wall_deflect").vol(1.0).speed(random_float(0.9, 1.1)).play();
            player.velocity_magnitude *= 0.95;
            player.health = 0;
            play_hit_bad();
            kill_player(terrain_segment_where_player_collides);
            //player.health *= 0.2;
            break;

         case COLOR_TYPE_GREEN:
            //sound("leaving").vol(0.4).play();
            //player.velocity_magnitude *= 0.95;
            if (delay_time_since_last_affect < 0)
            {
               delay_time_since_last_affect = 1.0f;
               player.health += 10.0;
               play_hit_soft();
            }
            if (player.health > player.max_health) player.health = player.max_health;
            break;

         case COLOR_TYPE_BLUE: // bouncy
            //sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
            //player.velocity_magnitude *= 0.95;
            if (delay_time_since_last_affect < 0)
            {
               play_hit_bounce();
               delay_time_since_last_affect = 1.0f;
               player.velocity_magnitude = 4.5;
            }
            break;

         case COLOR_TYPE_YELLOW:
            //sound("lamp").vol(0.1).speed(random_float(0.9, 1.1)).play();
            player.velocity_magnitude *= 0.95;
            if (delay_time_since_last_affect < 0)
            {
               play_hit_med();
               delay_time_since_last_affect = 1.0f;
               player.health -= 4;
               if (player.health < 0) { player.health = 0; kill_player(terrain_segment_where_player_collides); }
            }
            break;

         //case COLOR_TYPE_OFF:
            //sound("click").play();
            //break;
         default:       
            player.velocity_magnitude *= 0.95;
            //sound("wall_deflect").vol(racer->velocity_magnitude).speed(random_float(0.9, 1.1)).play();
            break;
         }
      }

      time_left_in_timestep -= (time_left_in_timestep*collision_time);
      num_steps++;
   }
   
   //if(debug) draw_crosshair(player_pos, al_color_name("red"));

   
   player.position.x = player_pos.x;
   player.position.y = player_pos.y;
   player.velocity.x = player_vel.x;
   player.velocity.y = player_vel.y;

   //if (player.y+player.h > 500)
   //{
      //player.y=500-player.h;
      //player.on_ground = true;
      //player.velocity_y = 0;
   //}
}








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



bool create_random_track(Track *t, int num_segments)
{
   t->clear();
   //int num_segments = 20;

   if (num_segments > 30) num_segments = 30;

   TrackSegment *track_segment = nullptr;

   track->create_gate(); // gate is added automatically

   segment_where_player_died.clear();

   track_segment = create_track_segmentE();
   track->append_segment(track_segment);
   //                   track_segment = create_track_segmentE();
                        //track->append_segment(track_segment);
                        //track->__HACK_finalize_track();
   //return;

   track_segment = create_track_segmentE();
   track->append_segment(track_segment);

   int num_consecutive_color_segments = 2;
   int consecutive_color_segment_ct = 0;
   int last_color_segment = 0;

   for (int i=0; i<num_segments; i++)
   {
      // pick the segment
      switch(random_int(0, 4))
      {
      case 0:
         track_segment = create_track_segmentA();
         break;
      case 1:
         track_segment = create_track_segmentB();
         break;
      case 2:
         track_segment = create_track_segmentC();
         break;
      case 3:
         track_segment = create_track_segmentD();
         break;
      case 4:
         track_segment = create_track_segmentE();
         break;
      }

      // pick a reverse flip of the segment or not
      if (random_bool()) track_segment->reverse();

      if ((consecutive_color_segment_ct%num_consecutive_color_segments) == 0)
      {
         int new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_MAX-1);

         if (num_segments == 4)
         {
            new_color_segment = COLOR_TYPE_YELLOW;
         }
         else if (num_segments == 10)
         {
            new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_GREEN);
         }
         else if (num_segments == 16)
         {
            // introduce blue, and unlikely red
            new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
            //if (new_color_segment == COLOR_TYPE_RED)
            // new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
         }
         else if (num_segments == 22)
         {
            // no yellow and unlikely green
            new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED_DEATH);
            while (new_color_segment == COLOR_TYPE_GREEN)
               new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
         }
         else if (num_segments == 28)
         {
            // no yellow or green, and more likely red
            new_color_segment = random_int(COLOR_TYPE_BLUE, COLOR_TYPE_RED_DEATH);
            if (new_color_segment != COLOR_TYPE_RED)
               new_color_segment = random_int(COLOR_TYPE_BLUE, COLOR_TYPE_RED_DEATH);
         }
         else if (num_segments == 30) new_color_segment = COLOR_TYPE_RED_DEATH;

         last_color_segment = new_color_segment;
      }

      consecutive_color_segment_ct++;

      track_segment->color_type = last_color_segment;

      // append the segment
      track->append_segment(track_segment);
   }

   track_segment = create_track_segmentE();
   track->append_segment(track_segment);

   track_segment = create_track_segmentE();
   track_segment->color_type = COLOR_TYPE_WHITE;
   track->append_segment(track_segment);

   track->__HACK_finalize_track();

   return true;
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

   OMG_DeltaTime = 0.6;

   particle_effect::reserve_particles(100);

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
   create_random_track(track, num_of_segments_in_track);
   start_track_begin_text();

   restart_music();

   racer = new Racer();

   camera = new CheapCamera();
   good_camera = new camera_class();

   fade_out_of_black();

   //start_track();
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
      //use_boost();
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
         create_random_track(track, num_of_segments_in_track);
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
         create_random_track(track, num_of_segments_in_track);
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



void game_timer_func(ALLEGRO_EVENT *current_event)
{
   motion.update(al_get_time());

   float screen_center_x = SCREEN_HW;
   float screen_center_y = SCREEN_HH;

   static ALLEGRO_FONT *font_regular = fonts["venus_rising_rg.ttf 28"];
   static ALLEGRO_FONT *font_large = fonts["venus_rising_rg.ttf 50"];


   start_profile_timer("WHOLE UPDATE");
   al_clear_to_color(al_color_name("black"));


   if (logo_showing)
   {
      ALLEGRO_BITMAP *logo_img = bitmaps.auto_get("lightracer-max-logo-02.png");
      ALLEGRO_COLOR logo_fade_opacity = al_map_rgba_f(1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity, 1.0 - foreground_black_opacity);
      al_draw_tinted_scaled_rotated_bitmap(logo_img, logo_fade_opacity, al_get_bitmap_width(logo_img)/2, al_get_bitmap_height(logo_img)/2,
         screen_center_x, (250 - 300) + screen_center_y, logo_scale, logo_scale, 0, 0);

      int font_size = -25;
      draw_text_with_letter_spacing(font_size, al_color_name("white"), screen_center_x, (375 - 300) + screen_center_y + 20, 12*2, fonts["venus_rising_rg.ttf 25"], "press ANY KEY to BEGIN");
      //al_draw_text(fonts["venus_rising_rg.ttf", -25), al_color_name("white"),
         //screen_center_x, (375 - 300) + screen_center_y + 20, ALLEGRO_ALIGN_CENTRE, "press ANY KEY to BEGIN");

      //draw_black_screen_overlay(al_map_rgba_f(0, 0, 0, foreground_black_opacity));
      return;
   }

   delay_time_since_last_affect -= 0.2;
   if (delay_time_since_last_affect < 0) delay_time_since_last_affect = -0.1f;




   start_profile_timer("racer/track");
   update_racer_and_track(racer, track); // < as I understand, *everything* that OMG_DeltaTime affects is in here
   stop_profile_timer("racer/track");


   camera->x = racer->position.x;
   camera->y = racer->position.y;
   camera->rotation = racer->direction_angle;
   camera->transform_on();


   good_camera->x = racer->position.x;
   good_camera->z = racer->position.y;

   al_set_sample_instance_speed(engine_sample_instance, 0.8+(racer->velocity_magnitude*racer->velocity_magnitude)/7);
   al_set_sample_instance_gain(engine_sample_instance, 0.6);

   start_profile_timer("particle update");
   particle_effect::update_all();
   stop_profile_timer("particle update");

   start_profile_timer("draw projected");
   track->draw_projected(camera, racer->direction_angle, racer->position.x, racer->position.y, racer->velocity_magnitude);
   stop_profile_timer("draw projected");

   draw_hud();


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
            ALLEGRO_COLOR color = al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter);
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

      if (FLAG_draw_profile_graph)
      {
         draw_profile_timer_graph(20, 20);
      }
         stop_profile_timer("WHOLE UPDATE");


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



#include "AllegroFlare/Framework.hpp"
#include "AllegroFlare/Screen.hpp"



class LightracerMax : public Screen
{
public:
   LightracerMax(Framework &framework, Screens &screens, Display *display)
      : Screen(framework, screens, display)
   {}

   void initialize()
   {
      init_game();
   }

   void primary_timer_func() override
   {
      game_timer_func(framework.current_event);
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
}
