

#include "Lightracer/GLRenderer.hpp"


#include "Lightracer/Racer.hpp"
#include "Lightracer/Track.hpp"
#include "allegro_flare/placement3d.h"


using allegro_flare::placement3d;


GLRenderer::GLRenderer()
{
}


void GLRenderer::setup_projection_SCENE(Camera3 &camera_to_use, ALLEGRO_BITMAP *backbuffer_sub_bitmap, ALLEGRO_TRANSFORM *transform_to_fill)
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


void GLRenderer::draw_gl_projection(Camera3 &camera3, Racer *racer, ALLEGRO_BITMAP *bitmap, ModelBin &models, Track *track)
{
   setup_projection_SCENE(camera3, bitmap, NULL);

   Model3D &cube_model = *models["rounded_unit_cube-01.obj"];
   cube_model.draw();

   placement3d place;
   float multiplier = 0.01;

   if (track)
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


