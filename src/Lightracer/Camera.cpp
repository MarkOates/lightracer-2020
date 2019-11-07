


#include "Lightracer/Camera.hpp"


#include <allegro5/allegro_color.h>
#include <allegro5/allegro_primitives.h>


Camera::Camera(int screen_w, int screen_h)
   : screen_w(screen_w)
   , screen_h(screen_h)
   , screen_hw(screen_w/2)
   , screen_hh(screen_h/2)
{
   //_z = 0.0;
   //z.set(&_z);

   depth = 128;  // the bigger this number, the less apparent the depth effect
   depth = 512;  // 512 is an accurate balance to where moving right 100px feels
                 // proportionally the same distance as moving forward 100px
   home_z = depth;
   z = home_z;
   center_point = vec2d(screen_w/2, screen_h/2);
   keyboard_controls_on = true;
}



float Camera::get_scale(float object_z)
{
   float distance_from_camera = z - object_z;
   if (distance_from_camera <= 0) return 0;
   if (depth <= 0) return 0;

   return depth / distance_from_camera;
}



vec2d Camera::get_displacement(float object_z)
{
   float scale = get_scale(object_z);
   return vec2d(-x * scale + screen_hw, -y * scale + screen_hh);
}



vec2d Camera::get_xy_displacement()
{
   return vec2d(-x + screen_hw, -y + screen_hh);
}



void Camera::draw_crosshair(float _x, float _y, float _z)
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



vec2d Camera::get_point(float _x, float _y, float _z)
{
   float scale = get_scale(_z);
   return vec2d((_x-x)*scale + center_point.x,  (_y-y)*scale + center_point.y);
}



void Camera::turn_on_controls()
{
   keyboard_controls_on = true;
}



void Camera::turn_off_controls()
{
   keyboard_controls_on = false;
}


