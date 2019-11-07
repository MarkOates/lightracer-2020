

#include <Lightracer/PlayerStats.hpp>



namespace Lightracer
{


PlayerStats::PlayerStats()
   : wall_hit_count(0)
   , throttle_release_count(0)
{
}


PlayerStats::~PlayerStats()
{
}


void PlayerStats::set_wall_hit_count(int wall_hit_count)
{
   this->wall_hit_count = wall_hit_count;
}


void PlayerStats::set_throttle_release_count(int throttle_release_count)
{
   this->throttle_release_count = throttle_release_count;
}


int PlayerStats::get_wall_hit_count()
{
   return wall_hit_count;
}


int PlayerStats::get_throttle_release_count()
{
   return throttle_release_count;
}


} // namespace Lightracer


