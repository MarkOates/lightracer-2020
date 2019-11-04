

#include "Lightracer/Track.hpp"
#include "Lightracer/TrackFactory.hpp"


using std::vector;

#include <allegro5/allegro_color.h> // for al_color_name

extern vector<int> segment_where_player_died;

void fill_track_rail_points();



Track::Track() : exit_segment_info(nullptr) {}




Track::~Track()
{
   for (int i=0; i<(int)segment.size(); i++) delete segment[i];
   segment.clear();
   segment_where_player_died.clear();
}



void Track::clear()
{
   for (int i=0; i<(int)segment.size(); i++)
      delete segment[i];
   segment.clear();
   segment_where_player_died.clear();
}



void Track::draw()
{
   for (int i=0; i<(int)segment.size(); i++)
      segment[i]->draw();

   al_draw_line(exit_p1.x, exit_p1.y, exit_p2.x, exit_p2.y, al_color_name("dodgerblue"), 20.0);

   al_draw_line(enter_p1.x, enter_p1.y, enter_p2.x, enter_p2.y, al_color_name("lightgreen"), 20.0);
}




//void draw_projected(CheapCamera *cam, float racer_facing_angle, float racer_x, float racer_y, float racer_speed);




void Track::update_exit_slope_info()
{
   if (exit_segment_info) delete exit_segment_info;
   exit_segment_info = new LineSegmentInfo(exit_p1, exit_p2);
}



void Track::__HACK_finalize_track()
{
   for (int i=0; i<(int)segment.size(); i++)
      segment[i]->__HACK_reverse_order_of_points();

   update_exit_slope_info();
   fill_track_rail_points();
}



void Track::make_segments_easy_where_player_died()
{
   for (int i=0; i<(int)segment_where_player_died.size(); i++)
   {
      segment[segment_where_player_died[i]]->color_type = COLOR_TYPE_OFF;

      //std::cout << "EASY: " << segment_where_player_died[i] << std::endl;
   }
   fill_track_rail_points();
}



void Track::create_gate()
{
   TrackSegment *gate = opening_gate();
   segment.push_back(gate);
}



void Track::append_segment(TrackSegment *ts)
{
   if (segment.empty()) { segment.push_back(ts); return; }

   vec2d pipe_output_vector = *segment.back()->right_rail.front()- *segment.back()->left_rail.back();
   float distance = pipe_output_vector.get_magnitude();

   ts->rotate(pipe_output_vector.get_angle());

   /*
   std::cout << "x: " << pipe_output_vector.x << std::endl;
   std::cout << "y: " << pipe_output_vector.y << std::endl;
   std::cout << "angle: " << pipe_output_vector.get_angle() << std::endl;
   std::cout << "angle(rad): " << degrees_to_radians(pipe_output_vector.get_angle()) << std::endl;
   std::cout << "mag: " << distance << std::endl;
   std::cout << std::endl;
   */
   //std::cout << "angle(deg): " << radians_to_degrees(pipe_output_vector.get_angle()) << std::endl;
   ts->move(segment.back()->left_rail.back()->x, segment.back()->left_rail.back()->y);

   segment.push_back(ts);

   //ts->__HACK_reverse_order_of_points();

   // update the entrance / exit
   exit_p1 = *segment.back()->left_rail.back();
   exit_p2 = *segment.back()->right_rail.front();
   enter_p1 = *segment.at(1)->left_rail.front(); // the edge of the gate
   enter_p2 = *segment.at(1)->right_rail.back(); // the edge of the gate
   //fill_track_rail_points();
}



vector<int> Track::__this_points_color__left_rail;
vector<int> Track::__this_points_color__right_rail;
vector<int> Track::__this_point_belongs_to_segment;
vector<vec2d> Track::track_rail_light; // < for caching
vector<vec2d> Track::track_rail_light_projected; // < for caching
vector<float> Track::car_distance_cache;
vector<std::pair<bool, int> > Track::rail_light_belongs_to;
vector<std::pair<bool, int> > Track::__left_color_light_belongs_to;
vector<std::pair<bool, int> > Track::__right_color_light_belongs_to;


