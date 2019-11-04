#pragma once


#include <utility> // for std::pair
#include <vector>
#include <allegro5/allegro_primitives.h> // for ALLEGRO_VERTEX
#include "AllegroFlare/Vec2D.hpp"
#include "Lightracer/LineSegmentInfo.hpp"
#include "Lightracer/TrackSegment.hpp"


class Racer;
class CheapCamera;

class Track
{
public:

   static std::vector<int> __this_points_color__left_rail;
   static std::vector<int> __this_points_color__right_rail;
   static std::vector<int> __this_point_belongs_to_segment;

   static std::vector<vec2d> track_rail_light; // < for caching
   static std::vector<std::pair<bool, int> > rail_light_belongs_to; // < for caching
   static std::vector<vec2d> track_rail_light_projected; // < for caching

   static std::vector<std::pair<bool, int> > __left_color_light_belongs_to; // < for caching
   static std::vector<std::pair<bool, int> > __right_color_light_belongs_to; // < for caching

   static std::vector<float> car_distance_cache;

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

   Track();
   ~Track();

   void clear();
   void draw();
   void draw_projected(float racer_facing_angle, float racer_x, float racer_y, float racer_speed);;
   void update_exit_slope_info();
   void __HACK_finalize_track();
   void make_segments_easy_where_player_died();
   void create_gate();
   void append_segment(TrackSegment *ts);
};
