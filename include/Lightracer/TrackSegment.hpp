#pragma once


#include "AllegroFlare/Vec2D.hpp"
#include "Lightracer/LineSegmentInfo.hpp"

#include <vector>



enum track_segmet_shape_t
{
   TRACK_SEGMENT_UNDEF = 0,
   TRACK_SEGMENT_A,
   TRACK_SEGMENT_B,
   TRACK_SEGMENT_C,
   TRACK_SEGMENT_D,
   TRACK_SEGMENT_E,
};



enum track_segment_color_t
{
   COLOR_TYPE_WHITE = 0,
   COLOR_TYPE_OFF,
   COLOR_TYPE_YELLOW,
   COLOR_TYPE_GREEN,
   COLOR_TYPE_BLUE,
   COLOR_TYPE_RED,
   COLOR_TYPE_RED_DEATH,
   COLOR_TYPE_MAX
};




using namespace AllegroFlare;


class TrackSegment
{
public:
   float w, h;

   float rotation;
   //vec2d displacement;

   std::vector<vec2d *> left_rail;
   std::vector<LineSegmentInfo *> left_rail_segments;
   float left_rail_length;

   std::vector<vec2d *> right_rail;
   std::vector<LineSegmentInfo *> right_rail_segments;
   float right_rail_length;


   vec2d *entrance_p1;
   vec2d *entrance_p2;
   LineSegmentInfo *entrance_segment_info;


   int color_type;

   int num_points();

   TrackSegment();

   ~TrackSegment();

   void add_point(int wall_num, float x, float y);

   void move(float x, float y);

   void rotate(float angle);

   void draw_projected();

   void reverse();

   void draw();

   void __HACK_reverse_order_of_points();

   void soften_left_rail();

   void soften_right_rail();

   void update_slope_info();
};



