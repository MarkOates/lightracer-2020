#pragma once


#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string>


namespace Banner
{
   class Base
   {
   protected:
      void draw_text_with_letter_spacing(ALLEGRO_COLOR color, int screen_hw, int x, int y, float letter_spacing, ALLEGRO_FONT *font, std::string string_to_write);

   public:
      Base();
      virtual ~Base();

      virtual void draw() = 0;
   };
}


