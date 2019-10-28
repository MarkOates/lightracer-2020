void game_timer_func()
{
   float screen_center_x = SCREEN_HW;
   float screen_center_y = SCREEN_HH;

   static ALLEGRO_FONT *font_regular = get_font("venus_rising_rg.ttf", -28);
   static ALLEGRO_FONT *font_large = get_font("venus_rising_rg.ttf", -50);

   if (logo_showing)
   {
      al_clear_to_color(color_hex("000000"));
      ALLEGRO_BITMAP *logo_img = bitmaps.auto_get("lightracer-max-logo-02.png");
      al_draw_scaled_rotated_bitmap(logo_img, al_get_bitmap_width(logo_img)/2, al_get_bitmap_height(logo_img)/2,
         screen_center_x, (250 - 300) + screen_center_y, logo_scale, logo_scale, 0, 0);

      int font_size = -25;
      draw_text_with_letter_spacing(font_size, al_color_name("white"), screen_center_x, (375 - 300) + screen_center_y + 20, 12*2, get_font("venus_rising_rg.ttf", font_size), "press ANY KEY to BEGIN");
      //al_draw_text(get_font("venus_rising_rg.ttf", -25), al_color_name("white"),
         //screen_center_x, (375 - 300) + screen_center_y + 20, ALLEGRO_ALIGN_CENTRE, "press ANY KEY to BEGIN");

      al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba_f(0, 0, 0, foreground_black_opacity));
      return;
   }

   delay_time_since_last_affect -= 0.2;
   if (delay_time_since_last_affect < 0) delay_time_since_last_affect = -0.1f;


   start_profile_timer("WHOLE UPDATE");
   al_clear_to_color(color_hex("000000"));



   start_profile_timer("racer/track");
   update_racer_and_track(racer, track); // < as I understand, *everything* that OMG_DeltaTime affects is in here
   stop_profile_timer("racer/track");


   int horizon_height = camera->rotation;
   al_draw_filled_rectangle(0, 0, horizon_height, SCREEN_W, al_color_name("gray"));

   camera->x = racer->position.x;
   camera->y = racer->position.y;
   camera->rotation = racer->direction_angle;
   camera->transform_on();


   good_camera->x = racer->position.x;
   good_camera->z = racer->position.y;

   al_set_sample_instance_speed(engine_sample_instance, 0.8+(racer->velocity_magnitude*racer->velocity_magnitude)/7);
   al_set_sample_instance_gain(engine_sample_instance, 0.6);

   start_profile_timer("particle update");
   particle_effect::update_all();
   stop_profile_timer("particle update");

   start_profile_timer("draw projected");
   track->draw_projected(camera, racer->direction_angle, racer->position.x, racer->position.y, racer->velocity_magnitude);
   stop_profile_timer("draw projected");

   draw_hud();


   ALLEGRO_TRANSFORM ident;
   al_identity_transform(&ident);
   al_use_transform(&ident);

   al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba_f(0, 0, 0, foreground_black_opacity));
   al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba_f(foreground_white_opacity, foreground_white_opacity, foreground_white_opacity, foreground_white_opacity));


   if (game_won)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "YOU WIN");
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "congratulations and");
      al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 300-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "thanks for playing!");
   }
   else if (game_over)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("red"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
      al_draw_text(font_regular, al_color_name("red"), screen_center_x, 350-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to START A NEW GAME");
   }
   else if (racer->dead)
   {
      stopwatch.pause();
      al_draw_text(font_large, al_color_name("orange"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "YOU FAILED");
      al_draw_text(font_regular, al_color_name("orange"), screen_center_x, 350-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
   }
   else
   {
      if (lap_notification_counter != 0 && ((racer->lap_time.size()+1) != 5))
      {

         std::string lap_string = "LAP " + tostring(racer->lap_time.size()+1);
         if ((int)racer->lap_time.size() == num_laps_to_win) lap_string = "FINAL LAP";


         int font_size = -30;
         draw_text_with_letter_spacing(font_size, al_color_name("white"), 0, screen_center_y - 160, 40 + 30 * (1.0 - lap_notification_counter), get_font("venus_rising_rg.ttf", font_size), lap_string.c_str());

         //al_draw_text(font_large, al_map_rgba_f(lap_notification_counter, lap_notification_counter, lap_notification_counter, lap_notification_counter),
            //screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_string.c_str());
      }

      if (track_begin_notification_counter > 0.4)
      {
         std::string lap_string = "READY";
         if (final_course) lap_string = "==-== FINAL COURSE ==-==";
         std::string lap_info_string = "track with " + tostring(num_of_segments_in_track) + " segments";
         std::string lap_info2_string = "with " + get_number_string(segment_where_player_died.size()) + " trouble spot";
         if (segment_where_player_died.size()==1) lap_info2_string += " removed";
         else lap_info2_string += "s removed";

         if (fmod(track_begin_notification_counter, 0.1f) < 0.05)
         {
            ALLEGRO_COLOR color = al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter);
            int font_size = -30;
            draw_text_with_letter_spacing(font_size, al_color_name("white"), 0, 250-300 + screen_center_y, 50, get_font("venus_rising_rg.ttf", font_size), lap_string.c_str());

            //al_draw_text(font_large, al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
               //screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_string.c_str());
         }


         draw_text_with_letter_spacing(-30, al_color_name("white"), screen_center_x, screen_center_y, 50, get_font("venus_rising_rg.ttf", -30), lap_info_string.c_str());

         //al_draw_text(font_large, al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
            //screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_info_string.c_str());

         if (!segment_where_player_died.empty())
            al_draw_text(font_regular, al_map_rgba_f(track_begin_notification_counter*0.5, track_begin_notification_counter, 0.0, track_begin_notification_counter),
               screen_center_x, 300-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, lap_info2_string.c_str());
      }
      else if (track_begin_notification_counter < 0.4 && (!(racer->velocity_magnitude > 0.01)))
      {
         std::string go_string = "GO";

         float opacity_counter = track_begin_notification_counter/0.2;

         al_draw_text(font_large, al_map_rgba_f(0.0, 1.0*opacity_counter, 0.0, opacity_counter),
            screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, go_string.c_str());
      }

      if (FLAG_draw_profile_graph)
      {
         draw_timer_profile_graph(20, 20, font_regular);
      }
         stop_profile_timer("WHOLE UPDATE");


      if (track_completed)
      {
         stopwatch.pause();
         al_draw_text(font_large, al_color_name("dodgerblue"), screen_center_x, 200-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "TRACK COMPLETED");
         al_draw_text(font_regular, al_color_name("dodgerblue"), screen_center_x, 250-300 + screen_center_y, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
         racer->velocity_magnitude *= 0.98;
         racer->throttle_on = false;
      }



   }


}

