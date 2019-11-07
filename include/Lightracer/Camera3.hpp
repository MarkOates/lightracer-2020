#pragma once


#include "AllegroFlare/Vec3D.hpp"
#include <allegro5/allegro.h>


using AllegroFlare::vec3d;


class Camera3
{
public:
	vec3d position;
	vec3d stepout;
	float spin;
	float tilt;

	Camera3();

	void position_transform(ALLEGRO_TRANSFORM *t);
	void reverse_position_transform(ALLEGRO_TRANSFORM *t);
	vec3d get_real_position();
};


