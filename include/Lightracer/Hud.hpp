#pragma once


#include "AllegroFlare/FontBin.hpp"
#include "AllegroFlare/Timer.hpp"


using namespace AllegroFlare; // for FontBin


class Hud
{
private:
   int num_of_segments_in_track;
   float current_racer_health;
   float current_racer_max_health;
   int current_lap_num;
   int num_laps_to_win;
   FontBin &fonts;
   int screen_width;
   Timer &stopwatch;
   int num_lives;
   int wall_hit_count;
   int throttle_release_count;
   int count_of_removed_track_segments;

   void draw_hud();
   void draw_num_segments(int text_x, int text_y, int num_segments);
   void draw_num_laps(int text_x, int text_y, int total_num_laps);
   void draw_wall_hit_count(int text_x, int text_y);
   void draw_throttle_release_count(int text_x, int text_y);
   void draw_count_of_removed_track_segments(int text_x, int text_y);
   void draw_stopwatch(int text_x, int text_y);
   void draw_lives(ALLEGRO_FONT *font, int text_x, int text_y);
   void draw_health_bar(ALLEGRO_FONT *font, int text_x, int text_y);
public:
   Hud(int num_of_segments_in_track, float current_racer_health, float current_racer_max_health, int current_lap_num, int num_laps_to_win, FontBin &fonts, int screen_width, Timer &stopwatch, int num_lives, int wall_hit_count, int throttle_release_count, int count_of_removed_track_segments);
   ~Hud();

   void draw();
};


