#pragma once


#include "AllegroFlare/Vec2D.hpp"

using namespace AllegroFlare;


class LineSegmentInfo
{
public:
   vec2d start;
   vec2d end;
   vec2d from_start;
   vec2d perpendicular;
   vec2d normal;
   vec2d middle;
   float radius;
   float length;

   LineSegmentInfo();
   LineSegmentInfo(vec2d &start, vec2d &end);
};


