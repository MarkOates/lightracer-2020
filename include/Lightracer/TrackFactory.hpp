#pragma once


#include "Lightracer/TrackSegment.hpp"
#include "Lightracer/Track.hpp"


class TrackFactory
{
private:
   static TrackSegment *create_track_segmentA();
   static TrackSegment *create_track_segmentB();
   static TrackSegment *create_track_segmentC();
   static TrackSegment *create_track_segmentD();
   static TrackSegment *create_track_segmentE();
   static TrackSegment *opening_gate();
   static TrackSegment *create_track_segment(track_segmet_shape_t shape_type, track_segment_color_t color_type, bool mirror, bool reverse);

public:

   static bool assemble_track(std::vector<int> &segment_where_player_died, Track *track, std::vector<std::tuple<track_segmet_shape_t, track_segment_color_t, bool, bool>> track_build_info);
   static bool create_classic_random_track(std::vector<int> &segment_where_player_died, Track *track, int num_segments);
   static bool create_random_track(std::vector<int> &segment_where_player_died, Track *track, int num_segments);
};


