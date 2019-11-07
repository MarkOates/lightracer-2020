

#include "Lightracer/Hud.hpp"


#include "AllegroFlare/TimerFormatter.hpp"
#include "AllegroFlare/Useful.hpp" // for tostring
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_color.h>
#include <sstream>


Hud::Hud(int num_of_segments_in_track, float current_racer_health, float current_racer_max_health, int current_lap_num, int num_laps_to_win, FontBin &fonts, int screen_width, Timer &stopwatch, int num_lives, int wall_hit_count, int throttle_release_count, int count_of_removed_track_segments)
   : num_of_segments_in_track(num_of_segments_in_track)
   , current_racer_health(current_racer_health)
   , current_racer_max_health(current_racer_max_health)
   , current_lap_num(current_lap_num)
   , num_laps_to_win(num_laps_to_win)
   , fonts(fonts)
   , screen_width(screen_width)
   , stopwatch(stopwatch)
   , num_lives(num_lives)
   , wall_hit_count(wall_hit_count)
   , throttle_release_count(throttle_release_count)
   , count_of_removed_track_segments(count_of_removed_track_segments)
{
}


Hud::~Hud()
{
}


void Hud::draw()
{
   draw_hud();
}


void Hud::draw_hud()
{
   ALLEGRO_TRANSFORM transform;
   al_identity_transform(&transform);
   al_use_transform(&transform);

   
  int text_y = 40;
  int horizontal_screen_padding = 100;

   draw_num_segments(text_y, num_of_segments_in_track);
   draw_wall_hit_count(text_y);
   draw_throttle_release_count(text_y);
   draw_count_of_removed_track_segments(text_y);

   draw_num_laps(text_y, num_laps_to_win);
   draw_stopwatch(text_y, horizontal_screen_padding);

   draw_health_bar(text_y, horizontal_screen_padding);
}


void Hud::draw_num_segments(int text_y, int num_segments)
{
   std::stringstream ss;
   ss << "SEG " << num_segments;

   std::string string_to_write = ss.str();

   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, screen_width - 300 - 264, text_y, ALLEGRO_ALIGN_LEFT, string_to_write.c_str());
}


void Hud::draw_wall_hit_count(int text_y)
{
   std::stringstream ss;
   ss << "WHC " << wall_hit_count;

   std::string string_to_write = ss.str();

   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, screen_width - 300 - 724, text_y, ALLEGRO_ALIGN_LEFT, string_to_write.c_str());
}


void Hud::draw_throttle_release_count(int text_y)
{
   std::stringstream ss;
   ss << "TRL " << throttle_release_count;

   std::string string_to_write = ss.str();

   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, screen_width - 300 - 924, text_y, ALLEGRO_ALIGN_LEFT, string_to_write.c_str());
}


void Hud::draw_count_of_removed_track_segments(int text_y)
{
   std::stringstream ss;
   ss << "RLF " << count_of_removed_track_segments;

   std::string string_to_write = ss.str();

   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, screen_width - 300 - 1124, text_y, ALLEGRO_ALIGN_LEFT, string_to_write.c_str());
}


void Hud::draw_num_laps(int text_y, int total_num_laps)
{
   std::string string_to_write = "";
   std::stringstream ss;

   ss << "LAP ";
   if (current_lap_num < total_num_laps)
   {
      ss << current_lap_num << " / " << total_num_laps;
   }
   else if (current_lap_num == total_num_laps)
   {
      ss << "FIN";
   }
   else
   {
      ss << "-";
   }
   string_to_write = ss.str();


   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");
   al_draw_text(font, color, screen_width - 300 - 300 - 200, text_y, ALLEGRO_ALIGN_LEFT, string_to_write.c_str());
}


void Hud::draw_stopwatch(int text_y, int horizontal_screen_padding)
{
   std::string ellapsed_time_str = std::string("TIM ") + TimerFormatter(stopwatch.get_elappsed_time_msec()).format();
   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];
   ALLEGRO_COLOR color = al_color_name("white");

   //float text_width = al_get_text_width(font, ellapsed_time_str.c_str());
   std::string the_char = " ";
   int letter_spacing = 12 * 2;
   int letters_traversed = 0;
   for (auto &c : ellapsed_time_str)
   {
      the_char[0] = c;
      al_draw_text(font, color, screen_width - 375 + (letters_traversed * letter_spacing), text_y, ALLEGRO_ALIGN_CENTER, the_char.c_str());
      letters_traversed++;
   }
}   


void Hud::draw_health_bar(int text_y, int horizontal_screen_padding)
{
   int health_bar_height = 10;
   ALLEGRO_FONT *font = fonts["venus_rising_rg.ttf 26"];

   std::string lives_string = "LIV " + tostring(num_lives);
   al_draw_text(font, al_color_name("white"), horizontal_screen_padding, text_y, 0, lives_string.c_str());

   float health_percentage = current_racer_health / current_racer_max_health;

   ALLEGRO_COLOR health_bar_color;
   if (health_percentage > 0.9) health_bar_color = al_color_name("dodgerblue");
   else if (health_percentage > 0.6) health_bar_color = al_color_name("violet");
   else if (health_percentage > 0.3) health_bar_color = al_color_name("orange");
   else health_bar_color = al_color_name("red");

   int lives_text_length = 120;
   int bar_length = 160;
   int bar_y = text_y + al_get_font_ascent(font) / 2 + 4;
   int bar_x = horizontal_screen_padding + lives_text_length + 4;

   al_draw_rounded_rectangle(
         bar_x - 4,
         bar_y - 4 - health_bar_height * 0.5,
         bar_x + 4 + bar_length,
         bar_y + 4 + health_bar_height * 0.5,
         4, 4,
         al_color_name("lightblue"), 2.0);

   al_draw_filled_rectangle(
         bar_x,
         bar_y - health_bar_height * 0.5,
         bar_x + bar_length * health_percentage,
         bar_y + health_bar_height * 0.5,
         al_color_name("lightblue"));
}


