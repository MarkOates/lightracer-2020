#pragma once


#include "Lightracer/Banner/Base.hpp"


#include "AllegroFlare/FontBin.hpp"
using AllegroFlare::FontBin;


namespace Banner
{
   class TrackCompleted : public Banner::Base
   {
   private:
      ALLEGRO_FONT *font_large;
      ALLEGRO_FONT *font_regular;
      float screen_center_x;
      float screen_center_y;

   public:
      TrackCompleted(ALLEGRO_FONT *font_large, ALLEGRO_FONT *font_regular, float screen_center_x, float screen_center_y);
      ~TrackCompleted();

      void draw() override;
   };
}


