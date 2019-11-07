

#include "Lightracer/Track.hpp"
#include "Lightracer/TrackFactory.hpp"
#include "Lightracer/Camera.hpp"
#include "Lightracer/CheapCamera.hpp"
#include "Lightracer/Racer.hpp"

#include "AllegroFlare/Useful.hpp" // for FULL_ROTATION

using std::vector;

#include <allegro5/allegro_color.h> // for al_color_name

#include "Lightracer/shared.hpp" // for typedef char64_t

using allegro_flare::char64_t;

extern vector<int> segment_where_player_died;

namespace allegro_flare
{
   void start_profile_timer(const char64_t name);
   void stop_profile_timer(const char64_t name, PROFILE_TIMER_TIME_T at_time=al_get_time());
}

using allegro_flare::start_profile_timer;
using allegro_flare::stop_profile_timer;



#include <cmath>

static inline void rotate_point(vec2d *point, const vec2d &axis, float angle)
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



Track::Track() : exit_segment_info(nullptr) {}




Track::~Track()
{
   for (int i=0; i<(int)segment.size(); i++) delete segment[i];
   segment.clear();
   segment_where_player_died.clear();
}



void Track::clear()
{
   for (int i=0; i<(int)segment.size(); i++)
      delete segment[i];
   segment.clear();
   segment_where_player_died.clear();
}



void Track::draw()
{
   for (int i=0; i<(int)segment.size(); i++)
      segment[i]->draw();

   al_draw_line(exit_p1.x, exit_p1.y, exit_p2.x, exit_p2.y, al_color_name("dodgerblue"), 20.0);

   al_draw_line(enter_p1.x, enter_p1.y, enter_p2.x, enter_p2.y, al_color_name("lightgreen"), 20.0);
}



void Track::draw_projected(int &index_of_last_track_segment_that_collides, Racer *racer, BitmapBin &bitmaps, Camera *good_camera, CheapCamera *camera, float racer_direction_angle, float racer_x, float racer_y, float racer_speed)
{
   start_profile_timer("DP 1");
   //////////////////////////
   /* modify perspective angles with these!! */
   
   //vec2d camera_target_offset = vec2d::polar_coords(racer_direction_angle, 50);

   vec2d racer_non_adjusted_pos(racer_x, racer_y);

   //racer_x += camera_target_offset.x;
   //racer_y += camera_target_offset.y;
   //racer_y -= 300;
   good_camera->z += 100 + 30*(4.0-racer_speed);

   float camera_y = 550 - 65*racer_speed;  // higher numbers (400) mean flatter, more birds-eye perspective
   float track_y_value = 50 + 50*(4.0-racer_speed) + (1.0-camera->zoom)*1000;
   //float multiplier = 0.15;

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
   static int NO_FLAGS = 0;




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
                              NO_FLAGS);

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
                              NO_FLAGS);

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
                              NO_FLAGS);
      }

   }


   //////////////////////////




   yellow_ball = bitmaps.auto_get("upbeam_light.png");


   //// DRAW PARTICLES

   stop_profile_timer("DP 2");

   //std::cout << "drawing particles : " << particle_draw_count << std::endl;





   start_profile_timer("DP drawing");

   //// DRAW THE SMALL LIGHTS

   start_profile_timer("DP rail lights");

   yellow_ball = bitmaps.auto_get("upbeam_light.png");
   ALLEGRO_BITMAP *rail_light_off = bitmaps.auto_get("light_off.png");

   //fill_track_rail_points();

   //std::cout << track_rail_point.size() << std::endl;

   //float distance_from_closest_light = 99999;
   //float distance_from_light = 0;

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
   //float bitmap_h_height_cache = al_get_bitmap_height(yellow_ball)*_align_y;

   start_profile_timer("DP rail drawing");
   //int draw_tinted = 0;
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
                                NO_FLAGS);

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
                                NO_FLAGS);
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
                                NO_FLAGS);
      
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

//void draw_projected(CheapCamera *cam, float racer_facing_angle, float racer_x, float racer_y, float racer_speed);




void Track::update_exit_slope_info()
{
   if (exit_segment_info) delete exit_segment_info;
   exit_segment_info = new LineSegmentInfo(exit_p1, exit_p2);
}



void Track::__HACK_finalize_track()
{
   for (int i=0; i<(int)segment.size(); i++)
      segment[i]->__HACK_reverse_order_of_points();

   update_exit_slope_info();
   fill_track_rail_points();
}



void Track::make_segments_easy_where_player_died()
{
   for (int i=0; i<(int)segment_where_player_died.size(); i++)
   {
      segment[segment_where_player_died[i]]->color_type = COLOR_TYPE_OFF;

      //std::cout << "EASY: " << segment_where_player_died[i] << std::endl;
   }
   fill_track_rail_points();
}



void Track::create_gate()
{
   TrackSegment *gate = opening_gate();
   segment.push_back(gate);
}



void Track::append_segment(TrackSegment *ts)
{
   if (segment.empty()) { segment.push_back(ts); return; }

   vec2d pipe_output_vector = *segment.back()->right_rail.front()- *segment.back()->left_rail.back();

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



void Track::fill_track_rail_points()
{
   Track *track = this;

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
         total_length += track->segment[segment_num]->left_rail_segments[rail]->length;
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
         total_length += track->segment[segment_num]->right_rail_segments[rail]->length;
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



vec2d Track::get_dot_at_distance(int track_segment, float distance, bool left)
{
   Track *track = this;

   // does left rail only
   if (left)
   {
      float length_traversed = 0;
      //for (int i=0; i<(int)track->segment.size(); i++)
      //{
         for (int rail=1; rail<(int)track->segment[track_segment]->left_rail.size(); rail++)
         {
            float segment_length = track->segment[track_segment]->left_rail_segments[rail]->length;
            //std::cout << "segment_length: " << segment_length << std::endl;

            if (distance <= (length_traversed + segment_length)
               && distance >= (length_traversed))
            {
               // dot is on this segment
               float dot_distance_along_segment = distance - length_traversed;
               //std::cout << "YAY";
               return track->segment[track_segment]->left_rail_segments[rail]->from_start.normalized() * dot_distance_along_segment
                  + track->segment[track_segment]->left_rail_segments[rail]->start;
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
            float segment_length = track->segment[track_segment]->right_rail_segments[rail]->length;
            //std::cout << "segment_length: " << segment_length << std::endl;

            if (distance <= (length_traversed + segment_length)
               && distance >= (length_traversed))
            {
               // dot is on this segment
               float dot_distance_along_segment = distance - length_traversed;
               //std::cout << "YAY";
               return track->segment[track_segment]->right_rail_segments[rail]->from_start.normalized() * dot_distance_along_segment
                  + track->segment[track_segment]->right_rail_segments[rail]->start;
            }

            length_traversed += segment_length;
         }

         return vec2d(-9999, -9999);
}



vector<int> Track::__this_points_color__left_rail;
vector<int> Track::__this_points_color__right_rail;
vector<int> Track::__this_point_belongs_to_segment;
vector<vec2d> Track::track_rail_light; // < for caching
vector<vec2d> Track::track_rail_light_projected; // < for caching
vector<float> Track::car_distance_cache;
vector<std::pair<bool, int> > Track::rail_light_belongs_to;
vector<std::pair<bool, int> > Track::__left_color_light_belongs_to;
vector<std::pair<bool, int> > Track::__right_color_light_belongs_to;


