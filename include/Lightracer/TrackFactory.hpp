#pragma once


#include "Lightracer/TrackSegment.hpp"
#include "Lightracer/Track.hpp"



TrackSegment *create_track_segmentA();

TrackSegment *create_track_segmentB();

TrackSegment *create_track_segmentC();

TrackSegment *create_track_segmentD();

TrackSegment *create_track_segmentE();

TrackSegment *opening_gate();

bool create_random_track(std::vector<int> &segment_where_player_died, Track *track, int num_segments);


