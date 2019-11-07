#pragma once


#include "Lightracer/Camera3.hpp"
#include "AllegroFlare/ModelBin.hpp"
#include <allegro5/allegro.h>


class Racer;
class Track;


using AllegroFlare::ModelBin;


class GLRenderer
{
private:
   void setup_projection_SCENE(Camera3 &camera_to_use, ALLEGRO_BITMAP *backbuffer_sub_bitmap, ALLEGRO_TRANSFORM *transform_to_fill=nullptr);

public:
   GLRenderer();

   void draw_gl_projection(Camera3 &camera3, Racer *racer, ALLEGRO_BITMAP *bitmap, ModelBin &models, Track *track_);
};


