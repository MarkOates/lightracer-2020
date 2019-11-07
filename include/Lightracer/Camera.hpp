#pragma once


#include "AllegroFlare/Vec2D.hpp"
#include <allegro5/allegro.h>


using namespace AllegroFlare; // for vec2d


class Camera : public vec2d
{
private:
   int screen_w;
   int screen_h;
   int screen_hw;
   int screen_hh;

public:
   float z;

   //animatable<float> z;
   float depth;
   vec2d center_point;
   float home_z;

   bool keyboard_controls_on;

   Camera(int screen_w, int screen_h);

   ALLEGRO_COLOR foreground_color; 

   float get_scale(float object_z);
   vec2d get_displacement(float object_z);
   vec2d get_xy_displacement();
   void draw_crosshair(float _x, float _y, float _z);
   vec2d get_point(float _x, float _y, float _z);
   void turn_on_controls();
   void turn_off_controls();
};


