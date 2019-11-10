#pragma once


#include "Lightracer/Banner/Base.hpp"


#include "AllegroFlare/FontBin.hpp"
using AllegroFlare::FontBin;


namespace Banner
{
   class Lap : public Banner::Base
   {
   private:
      ALLEGRO_FONT *font_large;
      ALLEGRO_FONT *font_regular;
      float screen_center_x;
      float screen_center_y;
      int lap_number;

   public:
      Lap(ALLEGRO_FONT *font_large, ALLEGRO_FONT *font_regular, float screen_center_x, float screen_center_y, int lap_number);
      ~Lap();

      void draw() override;
   };
}


