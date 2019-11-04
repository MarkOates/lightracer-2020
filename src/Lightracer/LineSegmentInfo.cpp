

#include "Lightracer/LineSegmentInfo.hpp"

#include "AllegroFlare/Useful.hpp" // for distance


LineSegmentInfo::LineSegmentInfo()
{
}


LineSegmentInfo::LineSegmentInfo(vec2d &start, vec2d &end)
{
   this->start = start;
   this->end = end;
   from_start = end - start;
   middle = from_start/2 + start;
   length = distance(start, end);
   radius = length/2;
   perpendicular = vec2d(-from_start.y, from_start.x);
   normal = ~-perpendicular;
}


