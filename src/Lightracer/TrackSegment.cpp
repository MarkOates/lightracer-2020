
#include "Lightracer/TrackSegment.hpp"


#include "Lightracer/shared.hpp"
#include "AllegroFlare/Useful.hpp" // for rotate_point
#include <allegro5/allegro_color.h> // for al_color_name
#include <allegro5/allegro_primitives.h> // for al_color_name


int TrackSegment::num_points()
{
   return right_rail.size() + left_rail.size();
}

TrackSegment::TrackSegment() : color_type(COLOR_TYPE_YELLOW), entrance_p1(nullptr), entrance_p2(nullptr), entrance_segment_info(nullptr)
 {}

TrackSegment::~TrackSegment()
{
   for (int i=0; i<(int)left_rail.size(); i++) delete left_rail[i];
   for (int i=0; i<(int)right_rail.size(); i++) delete right_rail[i];
   left_rail.clear();
   right_rail.clear();
   update_slope_info();
}

void TrackSegment::add_point(int wall_num, float x, float y)
{
   if (wall_num==LEFT_RAIL) left_rail.push_back(new vec2d(x, y));
   if (wall_num==RIGHT_RAIL) right_rail.push_back(new vec2d(x, y));
   //update_slope_info();
}

void TrackSegment::move(float x, float y)
{
   for (int i=0; i<(int)left_rail.size(); i++)
   {
      left_rail[i]->x += x;
      left_rail[i]->y += y;
   }
   for (int i=0; i<(int)right_rail.size(); i++)
   {
      right_rail[i]->x += x;
      right_rail[i]->y += y;
   }
   update_slope_info();
}

void TrackSegment::rotate(float angle)
{
   for (int i=0; i<(int)left_rail.size(); i++)
   {
      *left_rail[i] = rotate_point(*left_rail[i], angle);
   }
   for (int i=0; i<(int)right_rail.size(); i++)
   {
      *right_rail[i] = rotate_point(*right_rail[i], angle);
   }
   update_slope_info();
}

//void TrackSegment::draw_projected();

void TrackSegment::reverse()
{
   // doesn't work (yet)
   update_slope_info();
}

void TrackSegment::draw()
{
   int line_thickness = 3;

   for (int i=1; i<(int)left_rail.size(); i++)
      al_draw_line(left_rail[i-1]->x, left_rail[i-1]->y, left_rail[i]->x, left_rail[i]->y, al_color_name("orange"), 2.0*line_thickness);

   for (int i=1; i<(int)left_rail.size(); i++)
      al_draw_filled_circle(left_rail[i-1]->x, left_rail[i-1]->y, 2.0*line_thickness/2, al_color_name("orange"));

   for (int i=1; i<(int)right_rail.size(); i++)
      al_draw_line(right_rail[i-1]->x, right_rail[i-1]->y, right_rail[i]->x, right_rail[i]->y, al_color_name("orange"), 2.0*line_thickness);

   for (int i=1; i<(int)right_rail.size(); i++)
      al_draw_filled_circle(right_rail[i-1]->x, right_rail[i-1]->y, 2.0*line_thickness/2, al_color_name("orange"));

   return;

   al_draw_circle(left_rail.front()->x, left_rail.front()->y, 12, al_color_name("green"), 3.0);
   al_draw_circle(left_rail.back()->x, left_rail.back()->y, 6, al_color_name("red"), 3.0);
}

void TrackSegment::__HACK_reverse_order_of_points()
{
   std::reverse(left_rail.begin(), left_rail.end());
   std::reverse(right_rail.begin(), right_rail.end());
   std::reverse(left_rail_segments.begin(), left_rail_segments.end());
   std::reverse(right_rail_segment.begin(), right_rail_segment.end());
   update_slope_info();
}

void TrackSegment::soften_left_rail()
{
   if (left_rail.size() <= 2) return;

   std::vector<vec2d *> *newpoints = new std::vector<vec2d *>;
   
   // break each segment into 4 seperate segments
   for (int i=1; i<(int)left_rail.size(); i++)
   {
      newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.0 + *left_rail[i-1]));
      newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.25 + *left_rail[i-1]));
      newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.5 + *left_rail[i-1]));
      newpoints->push_back(new vec2d((*left_rail[i]-*left_rail[i-1])*0.75 + *left_rail[i-1]));
   }
   newpoints->push_back(new vec2d(*left_rail[left_rail.size()-1]));

   
   // skip the first 2
   // erase every other point
   bool keep = true;
   for (int i=1; i<(int)newpoints->size(); i++)
   {
      if (!keep)
      {
         delete newpoints->at(i);
         newpoints->erase(newpoints->begin()+i);
         i--;
      }
      keep = !keep;
   }
   newpoints->push_back(new vec2d(*left_rail[left_rail.size()-1]));

   delete newpoints->at(1);
   newpoints->erase(newpoints->begin()+1);

   delete newpoints->at((int)newpoints->size()-2);
   newpoints->erase(newpoints->end()-2);


   for (int i=0; i<(int)left_rail.size(); i++) delete left_rail[i];
   left_rail.clear();
   left_rail = *newpoints;

   update_slope_info();
}

void TrackSegment::soften_right_rail()
{
   if (right_rail.size() <= 2) return;

   std::vector<vec2d *> *newpoints = new std::vector<vec2d *>;
   
   // break each segment into 4 seperate segments
   for (int i=1; i<(int)right_rail.size(); i++)
   {
      newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.0 + *right_rail[i-1]));
      newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.25 + *right_rail[i-1]));
      newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.5 + *right_rail[i-1]));
      newpoints->push_back(new vec2d((*right_rail[i]-*right_rail[i-1])*0.75 + *right_rail[i-1]));
   }
   newpoints->push_back(new vec2d(*right_rail[right_rail.size()-1]));

   
   // skip the first 2
   // erase every other point
   bool keep = true;
   for (int i=1; i<(int)newpoints->size(); i++)
   {
      if (!keep)
      {
         delete newpoints->at(i);
         newpoints->erase(newpoints->begin()+i);
         i--;
      }
      keep = !keep;
   }
   newpoints->push_back(new vec2d(*right_rail[right_rail.size()-1]));

   delete newpoints->at(1);
   newpoints->erase(newpoints->begin()+1);

   delete newpoints->at((int)newpoints->size()-2);
   newpoints->erase(newpoints->end()-2);


   for (int i=0; i<(int)right_rail.size(); i++) delete right_rail[i];
   right_rail.clear();
   right_rail = *newpoints;

   update_slope_info();
}

void TrackSegment::update_slope_info()
{
   // for the left //

   if (left_rail.size() <= 1) return;
   for (int i=0; i<(int)left_rail_segments.size(); i++) delete left_rail_segments[i];

   left_rail_segments.clear();
   left_rail_segments.push_back(new LineSegmentInfo());
   left_rail_length = 0;

   for (int i=1; i<(int)left_rail.size(); i++)
   {
      left_rail_segments.push_back(new LineSegmentInfo(*left_rail[i-1], *left_rail[i]));
      left_rail_length += left_rail_segments.back()->length;
   }

   // for the right //

   if (right_rail.size() <= 1) return;
   for (int i=0; i<(int)right_rail_segment.size(); i++) delete right_rail_segment[i];

   right_rail_segment.clear();
   right_rail_segment.push_back(new LineSegmentInfo());
   right_rail_length = 0;

   for (int i=1; i<(int)right_rail.size(); i++)
   {
      right_rail_segment.push_back(new LineSegmentInfo(*right_rail[i-1], *right_rail[i]));
      right_rail_length += right_rail_segment.back()->length;
   }

   // entrance

   //if (entrance_p1) delete entrance_p1;
   //if (entrance_p2) delete entrance_p2;

   //entrance_p1 = new vec2d(0, 1);
   //entrance_p2 = new vec2d(1, 0);
   //entrance_segment_info = new LineSegmentInfo(*entrance_p1, *entrance_p2);

   entrance_p1 = left_rail.back();
   entrance_p2 = right_rail.front();
   if (entrance_segment_info) delete entrance_segment_info;
   entrance_segment_info = new LineSegmentInfo(*entrance_p1, *entrance_p2);
}



