#pragma once


#include "Lightracer/Camera3.hpp"
#include "AllegroFlare/ModelBin.hpp"
#include <allegro5/allegro.h>


class Racer;
class Track;


using AllegroFlare::ModelBin;


class GLRenderer
{
public:
   GLRenderer();

   void draw_gl_projection(ALLEGRO_DISPLAY *display, Camera3 &camera3, Racer *racer, ALLEGRO_BITMAP *bitmap, ModelBin &models, Track *track_);
};


