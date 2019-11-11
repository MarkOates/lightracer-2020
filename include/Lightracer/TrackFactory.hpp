#pragma once


#include "Lightracer/TrackSegment.hpp"
#include "Lightracer/Track.hpp"



TrackSegment *create_track_segmentA();

TrackSegment *create_track_segmentB();

TrackSegment *create_track_segmentC();

TrackSegment *create_track_segmentD();

TrackSegment *create_track_segmentE();

TrackSegment *opening_gate();

TrackSegment *create_track_segment(track_segmet_shape_t shape_type, track_segment_color_t color_type, bool mirror, bool reverse);

bool assemble_track(std::vector<int> &segment_where_player_died, Track *track, std::vector<std::tuple<track_segmet_shape_t, track_segment_color_t, bool, bool>> track_build_info);

bool create_random_track(std::vector<int> &segment_where_player_died, Track *track, int num_segments);


