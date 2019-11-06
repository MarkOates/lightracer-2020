

#include "Lightracer/Track.hpp"
#include "Lightracer/TrackFactory.hpp"


using std::vector;

#include <allegro5/allegro_color.h> // for al_color_name

extern vector<int> segment_where_player_died;
//extern vec2d get_dot_at_distance(int track_segment, float distance, bool left);



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



void Track::fill_track_rail_points()
{
   Track *track = this;

   vector<vec2d> &track_rail_point = Track::track_rail_light;
   //Track::track_rail_light.clear();
   track_rail_point.clear();
   Track::rail_light_belongs_to.clear();

   //Track::color_light_belongs_to.clear();





   // space the white lights:

   float total_length = 0;
   float dot_distance = 30;

   int segment_num = 0;
   for (int segment_num=0; segment_num<(int)track->segment.size(); segment_num++)
   {
      int rail = 1;
      for (; rail<(int)track->segment[segment_num]->left_rail.size(); rail++)
      {
         total_length += track->segment[segment_num]->left_rail_segments[rail]->length;
      }

      int d=0;
      do
      {
         track_rail_point.push_back(get_dot_at_distance(segment_num, d, true));
         Track::rail_light_belongs_to.push_back(std::pair<bool, int>(true, segment_num));
         d += dot_distance;
      }while (d<total_length);
   }


   // right rails

   total_length = 0;
   segment_num = 0;
   for (int segment_num=0; segment_num<(int)track->segment.size(); segment_num++)
   {
      int rail = 1;
      for (; rail<(int)track->segment[segment_num]->right_rail.size(); rail++)
      {
         total_length += track->segment[segment_num]->right_rail_segments[rail]->length;
      }

      int d=0;
      do
      {
         track_rail_point.push_back(get_dot_at_distance(segment_num, d, false));
         Track::rail_light_belongs_to.push_back(std::pair<bool, int>(false, segment_num));
         d += dot_distance;
      }while (d<total_length);
   }

   // match the "projected" cache and the car distance cache to the same size
   Track::track_rail_light_projected.clear();
   for (int i=0; i<(int)track_rail_point.size(); i++)
   {
      Track::track_rail_light_projected.push_back(vec2d(0,0));
      Track::car_distance_cache.push_back(0);
   }
}



vec2d Track::get_dot_at_distance(int track_segment, float distance, bool left)
{
   Track *track = this;

   // does left rail only
   if (left)
   {
      float length_traversed = 0;
      //for (int i=0; i<(int)track->segment.size(); i++)
      //{
         for (int rail=1; rail<(int)track->segment[track_segment]->left_rail.size(); rail++)
         {
            float segment_length = track->segment[track_segment]->left_rail_segments[rail]->length;
            //std::cout << "segment_length: " << segment_length << std::endl;

            if (distance <= (length_traversed + segment_length)
               && distance >= (length_traversed))
            {
               // dot is on this segment
               float dot_distance_along_segment = distance - length_traversed;
               //std::cout << "YAY";
               return track->segment[track_segment]->left_rail_segments[rail]->from_start.normalized() * dot_distance_along_segment
                  + track->segment[track_segment]->left_rail_segments[rail]->start;
            }

            length_traversed += segment_length;
         }
      //}

         return vec2d(-9999, -9999);
   }


      float length_traversed = 0;
      //for (int i=0; i<(int)track->segment.size(); i++)
      //{
         for (int rail=1; rail<(int)track->segment[track_segment]->right_rail.size(); rail++)
         {
            float segment_length = track->segment[track_segment]->right_rail_segments[rail]->length;
            //std::cout << "segment_length: " << segment_length << std::endl;

            if (distance <= (length_traversed + segment_length)
               && distance >= (length_traversed))
            {
               // dot is on this segment
               float dot_distance_along_segment = distance - length_traversed;
               //std::cout << "YAY";
               return track->segment[track_segment]->right_rail_segments[rail]->from_start.normalized() * dot_distance_along_segment
                  + track->segment[track_segment]->right_rail_segments[rail]->start;
            }

            length_traversed += segment_length;
         }

         return vec2d(-9999, -9999);
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


