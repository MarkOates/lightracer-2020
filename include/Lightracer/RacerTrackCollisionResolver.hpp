#pragma once


#include "Lightracer/Racer.hpp"
#include "Lightracer/Track.hpp"

#include "allegro5/allegro_audio.h"


class RacerTrackCollisionResolver
{
private:
   Racer &player;
   Track *track;

   double OMG_DeltaTime;
   int &index_of_last_track_segment_that_collides;
   ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance;
   int &num_laps_to_win;
   float &lap_notification_counter;
   Motion &motion;
   float &delay_time_since_last_affect;

public:
   RacerTrackCollisionResolver(
        Racer *racer
      , Track *track
      , int &index_of_last_track_segment_that_collides
      , ALLEGRO_SAMPLE_INSTANCE *passthough_sample_instance
      , int &num_laps_to_win
      , float &lap_notification_counter
      , Motion &motion
      , float &delay_time_since_last_affect
   );

   void update_player_velocity_and_direction();
   void update_racer_along_track();
   void resolve();
};


