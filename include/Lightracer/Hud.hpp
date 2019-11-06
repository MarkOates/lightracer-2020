#pragma once


#include "AllegroFlare/FontBin.hpp"
#include "AllegroFlare/Timer.hpp"
#include "Lightracer/Racer.hpp"


class Hud
{
private:
   int num_of_segments_in_track;
   Racer *racer;
   int current_lap_num;
   int num_laps_to_win;
   FontBin &fonts;
   int screen_width;
   Timer &stopwatch;
   int num_lives;

   void draw_hud();
   void draw_num_segments(int text_y, int num_segments);
   void draw_num_laps(int text_y, int total_num_laps);
   void draw_stopwatch(int text_y, int horizontal_screen_padding);
   void draw_health_bar(int text_y, int horizontal_screen_padding);
public:
   Hud(int num_of_segments_in_track, Racer *racer, int current_lap_num, int num_laps_to_win, FontBin &fonts, int screen_width, Timer &stopwatch, int num_lives);
   ~Hud();

   void draw();
};


