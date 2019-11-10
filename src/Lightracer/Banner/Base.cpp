

#include "Lightracer/Banner/Base.hpp"


#include <allegro5/allegro_primitives.h>


namespace Banner
{
   void Base::draw_frame(ALLEGRO_COLOR color, int x, int y, int x2, int y2, float radius, float thickness)
   {
      int health_bar_height = y2 - y;
      int bar_length = x2 - x;

      al_draw_rounded_rectangle(
            x - 4,
            y - 4 - health_bar_height * 0.5,
            x + 4 + bar_length,
            y + 4 + health_bar_height * 0.5,
            radius, radius,
            color,
            thickness
         );
   }


   void Base::draw_text_with_letter_spacing(ALLEGRO_COLOR color, int screen_hw, int x, int y, float letter_spacing, ALLEGRO_FONT *font, std::string string_to_write)
   {
      std::string the_char = " ";
      int num_characters = string_to_write.length();
      int letters_traversed = 0;
      int leftmost_character_x = screen_hw - (num_characters * letter_spacing) * 0.5;

      int font_ascent = al_get_font_ascent(font);

      for (auto &c : string_to_write)
      {
         the_char[0] = c;
         al_draw_text(font, color, leftmost_character_x + (letters_traversed * letter_spacing) + 30, y - font_ascent, ALLEGRO_ALIGN_CENTER, the_char.c_str());
         letters_traversed++;
      }
   }


   Base::Base()
   {
   }


   Base::~Base()
   {
   }
}


