void game_timer_func()
{
	if (logo_showing)
	{
		al_clear_to_color(color_hex("000000"));
		ALLEGRO_BITMAP *logo_img = bitmaps.auto_get("lightracer_logo.png");
		al_draw_scaled_rotated_bitmap(logo_img, al_get_bitmap_width(logo_img)/2, al_get_bitmap_height(logo_img)/2,
			400, 250, logo_scale, logo_scale, 0, 0);

			al_draw_text(get_font("lacuna.ttf", -20), al_color_name("white"),
				400, 325, ALLEGRO_ALIGN_CENTRE, "Use only the ARROW KEYS to play");
			al_draw_text(get_font("lacuna.ttf", -25), start_text_color,
				400, 375, ALLEGRO_ALIGN_CENTRE, "press ENTER to BEGIN");

			al_draw_text(get_font("lacuna.ttf", -20), al_color_name("white"),
				400, 500, ALLEGRO_ALIGN_CENTRE, "SpeedHack 2011 Entry by Mark Oates");


		al_draw_filled_rectangle(0, 0, SCREEN_W, SCREEN_H, al_map_rgba_f(0, 0, 0, foreground_black_opacity));
		return;
	}

	delay_time_since_last_affect -= 0.2;
	if (delay_time_since_last_affect < 0) delay_time_since_last_affect = -0.1f;


	start_profile_timer("WHOLE UPDATE");
	al_clear_to_color(color_hex("303030"));

	start_profile_timer("racer/track");
	update_racer_and_track(racer, track); // < as I understand, *everything* that OMG_DeltaTime affects is in here
	stop_profile_timer("racer/track");

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
		al_draw_text(get_font("lacuna.ttf", -50), al_color_name("dodgerblue"), 400, 200, ALLEGRO_ALIGN_CENTRE, "YOU WIN");
		al_draw_text(get_font("lacuna.ttf", -50), al_color_name("dodgerblue"), 400, 250, ALLEGRO_ALIGN_CENTRE, "congratulations and");
		al_draw_text(get_font("lacuna.ttf", -50), al_color_name("dodgerblue"), 400, 300, ALLEGRO_ALIGN_CENTRE, "thanks for playing!");
	}
	else if (game_over)
	{
		al_draw_text(get_font("lacuna.ttf", -50), al_color_name("red"), 400, 200, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
		al_draw_text(get_font("lacuna.ttf", -20), al_color_name("red"), 400, 250, ALLEGRO_ALIGN_CENTRE, "Get more contributers.");
      al_draw_text(get_font("lacuna.ttf", -30), al_color_name("red"), 400, 350, ALLEGRO_ALIGN_CENTRE, "press ENTER to START A NEW GAME");
	}
	else if (racer->dead)
	{
		al_draw_text(get_font("lacuna.ttf", -50), al_color_name("orange"), 400, 200, ALLEGRO_ALIGN_CENTRE, "YOU FAILED");
		al_draw_text(get_font("lacuna.ttf", -20), al_color_name("orange"), 400, 250, ALLEGRO_ALIGN_CENTRE, "Don't close your source code.");
      al_draw_text(get_font("lacuna.ttf", -20), al_color_name("orange"), 400, 350, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
	}
	else
	{
		if (lap_notification_counter != 0 && ((racer->lap_time.size()+1) != 5))
		{

			std::string lap_string = "LAP " + tostring(racer->lap_time.size()+1);
			if ((int)racer->lap_time.size() == num_laps_to_win) lap_string = "FINAL LAP";

			al_draw_text(get_font("lacuna.ttf", -40), al_map_rgba_f(lap_notification_counter, lap_notification_counter, lap_notification_counter, lap_notification_counter),
				400, 200, ALLEGRO_ALIGN_CENTRE, lap_string.c_str());
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
			al_draw_text(get_font("lacuna.ttf", -40), al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
				400, 200, ALLEGRO_ALIGN_CENTRE, lap_string.c_str());
			al_draw_text(get_font("lacuna.ttf", -40), al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
				400, 250, ALLEGRO_ALIGN_CENTRE, lap_info_string.c_str());
			if (!segment_where_player_died.empty())
				al_draw_text(get_font("lacuna.ttf", -20), al_map_rgba_f(track_begin_notification_counter*0.5, track_begin_notification_counter, 0.0, track_begin_notification_counter),
					400, 300, ALLEGRO_ALIGN_CENTRE, lap_info2_string.c_str());

			if (propeganda_on) 
				al_draw_text(get_font("lacuna.ttf", -20), al_map_rgba_f(track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter, track_begin_notification_counter),
					400, 350, ALLEGRO_ALIGN_CENTRE, "This game was made possible by open source.");
		}
		else if (track_begin_notification_counter < 0.4 && (!(racer->velocity_magnitude > 0.01)))
		{
			std::string go_string = "GO";

			float opacity_counter = track_begin_notification_counter/0.2;

			al_draw_text(get_font("lacuna.ttf", -40), al_map_rgba_f(0.0, 1.0*opacity_counter, 0.0, opacity_counter),
				400, 200, ALLEGRO_ALIGN_CENTRE, go_string.c_str());
		}

		if (FLAG_draw_profile_graph)
		{
			draw_timer_profile_graph(20, 20, get_font("lacuna.ttf", -19));
		}
			stop_profile_timer("WHOLE UPDATE");


		if (track_completed)
		{
			al_draw_text(get_font("lacuna.ttf", -50), al_color_name("dodgerblue"), 400, 200, ALLEGRO_ALIGN_CENTRE, "TRACK COMPLETED");
			al_draw_text(get_font("lacuna.ttf", -20), al_color_name("dodgerblue"), 400, 250, ALLEGRO_ALIGN_CENTRE, "press ENTER to continue");
			if (propeganda_on) al_draw_text(get_font("lacuna.ttf", -20), al_color_name("dodgerblue"), 400, 300, ALLEGRO_ALIGN_CENTRE, "It feels good to contribute....");
			racer->velocity_magnitude *= 0.98;
			racer->throttle_on = false;
		}



	}


}

