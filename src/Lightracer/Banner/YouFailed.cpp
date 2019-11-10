

#include "Lightracer/Banner/YouFailed.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>


namespace Banner
{
   YouFailed::YouFailed(ALLEGRO_FONT *font_large, ALLEGRO_FONT *font_regular, float screen_center_x, float screen_center_y)
      : Banner::Base()
      , font_large(font_large)
      , font_regular(font_regular)
      , screen_center_x(screen_center_x)
      , screen_center_y(screen_center_y)
   {
   }


   YouFailed::~YouFailed()
   {
   }


   void YouFailed::draw()
   {

      ALLEGRO_COLOR color = al_color_name("orange");
      int screen_w = 1920;
      int screen_hw = screen_w / 2; ///// PASS THIS DOWN

      float expected_text_width = screen_w - 500;
      int x = screen_hw - expected_text_width / 2;
      int x2 = x + expected_text_width;

      draw_frame(color, x, screen_center_y - 20, x2, screen_center_y + 50);
      draw_text_with_letter_spacing(color, screen_hw, screen_center_x+16, screen_center_y, 90, font_large, "YOU FAILED");
   }
}


