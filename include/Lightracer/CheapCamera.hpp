#pragma once


#include <allegro5/allegro.h>


class CheapCamera
{
public:
   float depth;
   float z;
   float x, y, rotation;
   float zoom;
   ALLEGRO_TRANSFORM transform;

   CheapCamera();

   void transform_on();
};


