#pragma once


#include "Lightracer/Banner/Base.hpp"


#include "AllegroFlare/FontBin.hpp"
using AllegroFlare::FontBin;


namespace Banner
{
   class YouFailed : public Banner::Base
   {
   private:
      ALLEGRO_FONT *font_large;
      ALLEGRO_FONT *font_regular;
      float screen_center_x;
      float screen_center_y;

   public:
      YouFailed(ALLEGRO_FONT *font_large, ALLEGRO_FONT *font_regular, float screen_center_x, float screen_center_y);
      ~YouFailed();

      void draw() override;
   };
}



