#pragma once


#include <utility> // for std::pair
#include <vector>
#include <allegro5/allegro_primitives.h> // for ALLEGRO_VERTEX
#include "AllegroFlare/Vec2D.hpp"
#include "AllegroFlare/BitmapBin.hpp"
#include "Lightracer/LineSegmentInfo.hpp"
#include "Lightracer/TrackSegment.hpp"


class Racer;
class CheapCamera;
class Camera;


class Track
{
public:

   std::vector<int> __this_points_color__left_rail;
   std::vector<int> __this_points_color__right_rail;
   std::vector<int> __this_point_belongs_to_segment;

   std::vector<vec2d> track_rail_light; // < for caching
   std::vector<std::pair<bool, int> > rail_light_belongs_to; // < for caching
   std::vector<vec2d> track_rail_light_projected; // < for caching

   std::vector<std::pair<bool, int> > __left_color_light_belongs_to; // < for caching
   std::vector<std::pair<bool, int> > __right_color_light_belongs_to; // < for caching

   std::vector<float> car_distance_cache;

   void fill_track_rail_points();
   vec2d get_dot_at_distance(int track_segment, float distance, bool left);

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
   void draw_projected(
      int &index_of_last_track_segment_that_collides,
      Racer *racer,
      BitmapBin &bitmaps,
      Camera *good_camera,
      CheapCamera *camera,
      float racer_facing_angle,
      float racer_x,
      float racer_y,
      float racer_speed
      );
   void update_exit_slope_info();
   void __HACK_finalize_track();
   void make_segments_easy_where_player_died();
   void append_segment(TrackSegment *ts);
};
