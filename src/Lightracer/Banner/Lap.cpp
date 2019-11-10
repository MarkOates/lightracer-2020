

#include "Lightracer/Banner/Lap.hpp"

#include <allegro5/allegro.h>
#include <allegro5/allegro_color.h>


namespace Banner
{
   Lap::Lap(ALLEGRO_FONT *font_large, ALLEGRO_FONT *font_regular, float screen_center_x, float screen_center_y, int lap_number)
      : Banner::Base()
      , font_large(font_large)
      , font_regular(font_regular)
      , screen_center_x(screen_center_x)
      , screen_center_y(screen_center_y)
      , lap_number(lap_number)
   {
   }


   Lap::~Lap()
   {
   }


   void Lap::draw()
   {
      ALLEGRO_COLOR color = al_color_html("#8afbfd"); //al_color_name("dodgerblue");
      int screen_w = 1920;
      int screen_hw = screen_w / 2;

      float expected_text_width = screen_w - 1600;
      int x = screen_hw - expected_text_width / 2;
      int x2 = x + expected_text_width;

      int y = screen_center_y - 10;

      std::string text_to_draw = std::string("LAP ") + std::to_string(lap_number);

      draw_frame(color, x, y, x2, y+40);
      draw_text_with_letter_spacing(color, screen_hw, screen_center_x+16, screen_center_y, 40, font_regular, text_to_draw);
   }
}


