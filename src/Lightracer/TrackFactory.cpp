#include "Lightracer/TrackFactory.hpp"

#include "Lightracer/shared.hpp"
#include "AllegroFlare/Useful.hpp" // for random_int, random_bool


TrackSegment *create_track_segmentA()
{
   TrackSegment *segment = new TrackSegment();
   segment->add_point(LEFT_RAIL, 200, 500);
   segment->add_point(LEFT_RAIL, 200, 375);
   segment->add_point(LEFT_RAIL, 150, 275);
   segment->add_point(LEFT_RAIL, 150, 100);

   segment->add_point(RIGHT_RAIL, 250, 100);
   segment->add_point(RIGHT_RAIL, 250, 250);
   segment->add_point(RIGHT_RAIL, 300, 350);
   segment->add_point(RIGHT_RAIL, 300, 500);

   segment->move(-200, -500);

   return segment;
}


TrackSegment *create_track_segmentB()
{
   TrackSegment *segment = new TrackSegment();
   segment->add_point(LEFT_RAIL, 300, 500);
   segment->add_point(LEFT_RAIL, 300, 400);
   segment->add_point(LEFT_RAIL, 350, 350);
   segment->add_point(LEFT_RAIL, 350, 200);
   segment->add_point(LEFT_RAIL, 250, 100);
   segment->add_point(LEFT_RAIL, 0, 100);

   segment->add_point(RIGHT_RAIL, 0, 0);
   segment->add_point(RIGHT_RAIL, 450, 0);
   segment->add_point(RIGHT_RAIL, 450, 450);
   segment->add_point(RIGHT_RAIL, 400, 500);

   segment->move(-300, -500);

   return segment;
}


TrackSegment *create_track_segmentC()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 0, 500);
   segment->add_point(LEFT_RAIL, 0, 125);
   segment->add_point(LEFT_RAIL, 250, 50);

   segment->add_point(RIGHT_RAIL, 300, 500-363.396);
   segment->add_point(RIGHT_RAIL, 125, 225);
   segment->add_point(RIGHT_RAIL, 100, 500);

   segment->move(0, -500);

   return segment;
}


TrackSegment *create_track_segmentD()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 200, 500);
   segment->add_point(LEFT_RAIL, 200, 200);
   segment->add_point(LEFT_RAIL, 100, 100);
   segment->add_point(LEFT_RAIL, 0, 100);

   segment->add_point(RIGHT_RAIL, 0, 0);
   segment->add_point(RIGHT_RAIL, 150, 0);
   segment->add_point(RIGHT_RAIL, 300, 150);
   segment->add_point(RIGHT_RAIL, 300, 500);

   segment->move(-200, -500);

   return segment;
}



TrackSegment *create_track_segmentE()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 50, 1000);
   segment->add_point(LEFT_RAIL, 50, 650);
   segment->add_point(LEFT_RAIL, 0, 600);
   segment->add_point(LEFT_RAIL, 0, 200);
   segment->add_point(LEFT_RAIL, 50, 150);
   segment->add_point(LEFT_RAIL, 50, 0);

   segment->add_point(RIGHT_RAIL, 150, 0);
   segment->add_point(RIGHT_RAIL, 150, 150);
   segment->add_point(RIGHT_RAIL, 200, 200);
   segment->add_point(RIGHT_RAIL, 200, 600);
   segment->add_point(RIGHT_RAIL, 150, 650);
   segment->add_point(RIGHT_RAIL, 150, 1000);

   segment->move(-50, -1000);

   return segment;
}



TrackSegment *opening_gate()
{
   TrackSegment *segment = new TrackSegment();

   segment->add_point(LEFT_RAIL, 0, 500);
   segment->add_point(LEFT_RAIL, -50, 500);
   segment->add_point(LEFT_RAIL, -100, 425);
   segment->add_point(LEFT_RAIL, -100, 325);
   segment->add_point(LEFT_RAIL, -50, 225);
   segment->add_point(LEFT_RAIL, -50, 125);

   segment->add_point(RIGHT_RAIL, 50, 125);
   segment->add_point(RIGHT_RAIL, 50, 225);
   segment->add_point(RIGHT_RAIL, 100, 325);
   segment->add_point(RIGHT_RAIL, 100, 425);
   segment->add_point(RIGHT_RAIL, 50, 500);
   segment->add_point(RIGHT_RAIL, 0, 500);

   segment->move(50, 475);

   return segment;
}


TrackSegment *create_track_segment(track_segmet_shape_t shape_type, track_segment_color_t color_type, bool mirror, bool reverse)
{
   TrackSegment *track_segment = nullptr;

   switch(shape_type)
   {
   case TRACK_SEGMENT_UNDEF:
      throw std::runtime_error("assemble_track cannot attach track segment of type TRACK_SEGMENT_UNDEF");
      break;
   case TRACK_SEGMENT_A:
      track_segment = create_track_segmentA();
      break;
   case TRACK_SEGMENT_B:
      track_segment = create_track_segmentB();
      break;
   case TRACK_SEGMENT_C:
      track_segment = create_track_segmentC();
      break;
   case TRACK_SEGMENT_D:
      track_segment = create_track_segmentD();
      break;
   case TRACK_SEGMENT_E:
      track_segment = create_track_segmentE();
      break;
   }

   track_segment->color_type = color_type;

   if (mirror) track_segment->mirror();
   if (reverse) track_segment->reverse();

   return track_segment;
}


bool assemble_track(std::vector<int> &segment_where_player_died, Track *track, std::vector<std::tuple<track_segmet_shape_t, track_segment_color_t, bool, bool>> track_build_info)
{
   track->clear();
   TrackSegment *track_segment = nullptr;
   track->create_gate(); // gate is added automatically
   segment_where_player_died.clear();
   track_segment = create_track_segmentE();
   track->append_segment(track_segment);


   /// build the track here
   for (auto &track_build_info_unit : track_build_info)
   {
      track_segmet_shape_t shape = std::get<0>(track_build_info_unit);//.first;
      track_segment_color_t color = std::get<1>(track_build_info_unit);//.second;
      bool mirror = std::get<2>(track_build_info_unit);
      bool reverse = std::get<3>(track_build_info_unit);

      track_segment = create_track_segment(shape, color, mirror, reverse);

      track->append_segment(track_segment);
   }


   track_segment = create_track_segmentE();
   track_segment->color_type = COLOR_TYPE_WHITE;
   track->append_segment(track_segment);

   track->__HACK_finalize_track();

   return true;
}


bool create_random_track(std::vector<int> &segment_where_player_died, Track *track, int num_segments)
{
   assemble_track(segment_where_player_died, track, {
         { TRACK_SEGMENT_B, COLOR_TYPE_WHITE, false, false },
         { TRACK_SEGMENT_B, COLOR_TYPE_YELLOW, false, false },
         { TRACK_SEGMENT_B, COLOR_TYPE_RED, true, false },
         { TRACK_SEGMENT_B, COLOR_TYPE_BLUE, true, false },
   });

   return true;


   track->clear();
   //int num_segments = 20;

   if (num_segments > 30) num_segments = 30;

   TrackSegment *track_segment = nullptr;

   track->create_gate(); // gate is added automatically

   segment_where_player_died.clear();

   track_segment = create_track_segmentE();
   track->append_segment(track_segment);
   //                   track_segment = create_track_segmentE();
                        //track->append_segment(track_segment);
                        //track->__HACK_finalize_track();
   //return;

   track_segment = create_track_segmentE();
   track->append_segment(track_segment);

   int num_consecutive_color_segments = 2;
   int consecutive_color_segment_ct = 0;
   int last_color_segment = 0;

   for (int i=0; i<num_segments; i++)
   {
      // pick the segment
      switch(random_int(0, 4))
      {
      case 0:
         track_segment = create_track_segmentA();
         break;
      case 1:
         track_segment = create_track_segmentB();
         break;
      case 2:
         track_segment = create_track_segmentC();
         break;
      case 3:
         track_segment = create_track_segmentD();
         break;
      case 4:
         track_segment = create_track_segmentE();
         break;
      }

      // pick a reverse flip of the segment or not
      if (random_bool()) track_segment->reverse();

      if ((consecutive_color_segment_ct%num_consecutive_color_segments) == 0)
      {
         int new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_MAX-1);

         if (num_segments == 4)
         {
            new_color_segment = COLOR_TYPE_YELLOW;
         }
         else if (num_segments == 10)
         {
            new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_GREEN);
         }
         else if (num_segments == 16)
         {
            // introduce blue, and unlikely red
            new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
            //if (new_color_segment == COLOR_TYPE_RED)
            // new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
         }
         else if (num_segments == 22)
         {
            // no yellow and unlikely green
            new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED_DEATH);
            while (new_color_segment == COLOR_TYPE_GREEN)
               new_color_segment = random_int(COLOR_TYPE_YELLOW, COLOR_TYPE_RED);
         }
         else if (num_segments == 28)
         {
            // no yellow or green, and more likely red
            new_color_segment = random_int(COLOR_TYPE_BLUE, COLOR_TYPE_RED_DEATH);
            if (new_color_segment != COLOR_TYPE_RED)
               new_color_segment = random_int(COLOR_TYPE_BLUE, COLOR_TYPE_RED_DEATH);
         }
         else if (num_segments == 30) new_color_segment = COLOR_TYPE_RED_DEATH;

         last_color_segment = new_color_segment;
      }

      consecutive_color_segment_ct++;

      track_segment->color_type = last_color_segment;

      // append the segment
      track->append_segment(track_segment);
   }

   track_segment = create_track_segmentE();
   track->append_segment(track_segment);

   track_segment = create_track_segmentE();
   track_segment->color_type = COLOR_TYPE_WHITE;
   track->append_segment(track_segment);

   track->__HACK_finalize_track();

   return true;
}


