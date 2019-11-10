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
   void draw_track(Track *track, float multiplier, ModelBin &models, int strobe, int index_of_last_track_segment_that_collides);

public:
   GLRenderer();

   void draw_gl_projection(ALLEGRO_DISPLAY *display, Camera3 &camera3, Racer *racer, ALLEGRO_BITMAP *bitmap, ModelBin &models, Track *track_, int index_of_last_track_segment_that_collides);
};


