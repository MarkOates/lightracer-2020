

#include "Lightracer/Camera3.hpp"


Camera3::Camera3()
   : position(0, 0, 0)
   , stepout(0, 0, 0)
   , spin(0)
   , tilt(0)
{}


void Camera3::position_transform(ALLEGRO_TRANSFORM *t)
{
   al_identity_transform(t);

   al_translate_transform_3d(t, stepout.x, stepout.y, stepout.z);
   al_rotate_transform_3d(t, -1, 0, 0, tilt);
   al_rotate_transform_3d(t, 0, -1, 0, spin);
}


void Camera3::reverse_position_transform(ALLEGRO_TRANSFORM *t)
{
   // note: this is EXACTLY the same as position transform, except the
   // order of transformations is reversed, and the values are negated
   al_identity_transform(t);

   al_rotate_transform_3d(t, 0, 1, 0, spin);
   al_rotate_transform_3d(t, 1, 0, 0, tilt);
   al_translate_transform_3d(t, -stepout.x, -stepout.y, -stepout.z);
}


vec3d Camera3::get_real_position()
{
   vec3d real_position(0, 0, 0);
   ALLEGRO_TRANSFORM t;

   position_transform(&t);
   al_transform_coordinates_3d(&t, &real_position.x, &real_position.y, &real_position.z);

   return real_position;
}


