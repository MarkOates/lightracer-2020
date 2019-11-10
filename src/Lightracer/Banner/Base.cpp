// this is a header only class


#include "Lightracer/Banner/Base.hpp"


namespace Banner
{
   void Base::draw_text_with_letter_spacing(ALLEGRO_COLOR color, int screen_hw, int x, int y, float letter_spacing, ALLEGRO_FONT *font, std::string string_to_write)
   {
      std::string the_char = " ";
      int num_characters = string_to_write.length();
      int letters_traversed = 0;
      int leftmost_character_x = screen_hw - (num_characters * letter_spacing) * 0.5;
      for (auto &c : string_to_write)
      {
         the_char[0] = c;
         al_draw_text(font, color, leftmost_character_x + (letters_traversed * letter_spacing), y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
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


