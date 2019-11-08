

#include "Lightracer/GLRenderer.hpp"


#include <allegro5/allegro_color.h>
#include "Lightracer/Racer.hpp"
#include "Lightracer/Track.hpp"
#include "AllegroFlare/Useful.hpp" // for FULL_ROTATION
#include "allegro_flare/placement3d.h"
#include "allegro_flare/camera3d.h"


#include <cmath> // for std::sin


using allegro_flare::placement3d;
using allegro_flare::Camera3D;


GLRenderer::GLRenderer()
{
}


void GLRenderer::draw_gl_projection(ALLEGRO_DISPLAY *display, Camera3 &camera3, Racer *racer, ALLEGRO_BITMAP *bitmap, ModelBin &models, Track *track)
{
   float multiplier = 0.07;
   // create a camera3d

   vec3d racer_position = vec3d(racer->position.x, 0, racer->position.y) * multiplier;
   vec3d racer_view_vector = vec3d(racer->direction.x, 0, racer->direction.y).normalized();

   vec3d targets_position = racer_position;
   AllegroFlare::vec3d view_vec(0, 0, 0);

   float ideal_velocity = 4.165;

   float moving_non_moving_normal = 1.0 - (racer->velocity_magnitude / ideal_velocity); // 0 is not moving, 1 is moving at te

   Camera3D camera3d(0);
   //camera3d.stepback = 30;
   //camera3d.stepback_rotation = al_get_time() * 0.2;
   //camera3d.pitch = 0.5 * std::sin(al_get_time());
         camera3d.is_fixed_on_axis = true;
         camera3d.position = targets_position;
         camera3d.position = targets_position + AllegroFlare::vec3d(0, -2, 0);
         view_vec = racer_view_vector; // this is a fixed track camera tracking, might be good for victory laps: AllegroFlare::vec3d(0, 0, -1);
         //view_vec = AllegroFlare::vec3d(0, 0, -1);
         camera3d.stepback = view_vec * -12;
         camera3d.stepback += AllegroFlare::vec3d(0, 16 - ideal_velocity*1.1, 0); // ascent
         camera3d.stepback *= (1.0 - 0.11 * ideal_velocity);
         camera3d.stepback += AllegroFlare::vec3d(0, 13 * moving_non_moving_normal, 4 * moving_non_moving_normal); // ascent
         camera3d.pitch = 0;
         camera3d.stepback_pitch = -1.2 + 0.17 * ideal_velocity;
         camera3d.stepback_pitch += -0.4 * moving_non_moving_normal; // ascent
         camera3d.stepback += AllegroFlare::vec3d(0, -1, 0); // ascent
         camera3d.view_vector = view_vec;

   camera3d.set_frustum_as_camera(display);


   ////setup_projection_SCENE(camera3, bitmap, NULL);
   //ALLEGRO_BITMAP *backbuffer_sub_bitmap = bitmap;
   //Camera3 &camera_to_use = camera3;

   //// setup the render settings
   //al_set_render_state(ALLEGRO_DEPTH_TEST, 1);
   //al_set_render_state(ALLEGRO_WRITE_MASK, ALLEGRO_MASK_DEPTH | ALLEGRO_MASK_RGBA);
   //al_clear_depth_buffer(1);

   //ALLEGRO_TRANSFORM t;

   ////camera_to_use.reverse_position_transform(&t);

   //float aspect_ratio = (float)al_get_bitmap_height(backbuffer_sub_bitmap) / al_get_bitmap_width(backbuffer_sub_bitmap);
   //al_perspective_transform(&t, -1, aspect_ratio, 1, 1, -aspect_ratio, 100);

   //al_use_projection_transform(&t);




   multiplier = 0.07;
   Model3D &cube_model = *models["rounded_unit_cube-01.obj"];
   //cube_model.draw();

   placement3d place;
   placement3d lamp_placement;

   lamp_placement.scale = vec3d(0.2, 0.2, 0.8);
   lamp_placement.size = vec3d(1, 1, 1);
   lamp_placement.align = vec3d(0, 0, 0.5);

   float height_above_ground;

   if (track)
   {
      place.rotation.z = 0.5;// / 2.0 + al_get_time() * 0.04;
      place.rotation.x = 0.25; //+ al_get_time() * 0.04;
      place.rotation.y = 0.5; //+ al_get_time() * 0.04;
      place.start_transform();

      TrackSegment *first_track_segment = track->segment.empty() ? nullptr : track->segment[0];
      for (auto &track_segment : track->segment)
      {
         // draw_the_left_rail
         for (unsigned i=1; i<track_segment->left_rail_segments.size(); i++)
         {
            LineSegmentInfo *segment = track_segment->left_rail_segments[i];
            vec2d start = segment->start * multiplier;
            vec2d end = segment->end * multiplier;
            al_draw_line(start.x, start.y, end.x, end.y, al_color_name("white"), 3 * multiplier);

            lamp_placement.position = vec3d(start.x, start.y, -0.5);
            lamp_placement.start_transform();
            cube_model.draw();
            lamp_placement.restore_transform();
         }

         // draw_the_right_rail
         for (unsigned i=1; i<track_segment->right_rail_segments.size(); i++)
         {
            LineSegmentInfo *segment = track_segment->right_rail_segments[i];
            vec2d start = segment->start * multiplier;
            vec2d end = segment->end * multiplier;
            al_draw_line(start.x, start.y, end.x, end.y, al_color_name("white"), 3 * multiplier);

            lamp_placement.position = vec3d(start.x, start.y, -0.5);
            lamp_placement.start_transform();
            cube_model.draw();
            lamp_placement.restore_transform();
         }
      }

      place.restore_transform();


      //vec3d exit_p1_pos = vec3d(track->exit_p1.x * multiplier, 0, track->exit_p1.y * multiplier);
      //vec3d exit_p2_pos = vec3d(track->exit_p2.x * multiplier, 0, track->exit_p2.y * multiplier);

      //place.position = exit_p1_pos;
      //place.start_transform();
      //cube_model.draw();
      //place.restore_transform();

      //place.position = exit_p2_pos;
      //place.start_transform();
      //cube_model.draw();
      //place.restore_transform();

      //vec3d enter_p1_pos = vec3d(track->enter_p1.x * multiplier, 0, track->enter_p1.y * multiplier);
      //vec3d enter_p2_pos = vec3d(track->enter_p2.x * multiplier, 0, track->enter_p2.y * multiplier);

      //place.position = enter_p1_pos;
      //place.start_transform();
      //cube_model.draw();
      //place.restore_transform();

      //place.position = enter_p2_pos;
      //place.start_transform();
      //cube_model.draw();
      //place.restore_transform();

   }
   
   // draw the racer

   if (racer)
   {
      vec3d racer_pos = vec3d(racer->position.x * multiplier, 0, racer->position.y * multiplier);
      place.rotation = vec3d(0, -radians_to_degrees(racer->direction.get_angle()/180/2), 0);
      place.position = racer_pos;
      place.start_transform();
      cube_model.draw();
      place.restore_transform();
   }
}


