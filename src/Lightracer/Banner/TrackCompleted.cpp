

#include "Lightracer/Banner/TrackCompleted.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>


namespace Banner
{
   TrackCompleted::TrackCompleted(ALLEGRO_FONT *font_large, ALLEGRO_FONT *font_regular, float screen_center_x, float screen_center_y)
      : Banner::Base()
      , font_large(font_large)
      , font_regular(font_regular)
      , screen_center_x(screen_center_x)
      , screen_center_y(screen_center_y)
   {
   }


   TrackCompleted::~TrackCompleted()
   {
   }


   void TrackCompleted::draw()
   {
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "TRACK COMPLETED");
      al_draw_text(font_regular, al_color_name("dodgerblue"), screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
   }
}


