

#include "Lightracer/CheapCamera.hpp"


CheapCamera::CheapCamera()
{
   depth = 512;
   z = 256;
   rotation = 0.0;
   zoom = 1.0;
}


void CheapCamera::transform_on()
{
   al_identity_transform(&transform);
   al_translate_transform(&transform, -x, -y);
   al_scale_transform(&transform, zoom, zoom);
   //al_rotate_transform(&transform, rotation+FULL_ROTATION/2);
   al_translate_transform(&transform, 400, 300);
   //al_build_transform(&transform, x, y, 1.0, 1.0, rotation);
   al_use_transform(&transform);
}


