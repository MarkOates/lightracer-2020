

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
      ALLEGRO_COLOR color = al_color_name("dodgerblue");
      int screen_hw = 1920 / 2; ///// PASS THIS DOWN

      draw_text_with_letter_spacing(color, screen_hw, screen_center_x, screen_center_y - 50, 90, font_large, "TRACK COMPLETED");
      draw_text_with_letter_spacing(color, screen_hw, screen_center_x, screen_center_y + 50, 50, font_regular, "press ENTER to continue");

      //al_draw_text(font_large, color, screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "TRACK COMPLETED");
      //al_draw_text(font_regular, color, screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
   }
}


