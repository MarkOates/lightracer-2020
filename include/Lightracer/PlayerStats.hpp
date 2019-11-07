#pragma once




namespace Lightracer
{
   class PlayerStats
   {
   private:
      int wall_hit_count;
      int throttle_release_count;

   public:
      PlayerStats();
      ~PlayerStats();

      void set_wall_hit_count(int wall_hit_count);
      void set_throttle_release_count(int throttle_release_count);

      int get_wall_hit_count();
      int get_throttle_release_count();

      void increment_wall_hit_count();
      void increment_throttle_release_count();
   };
}



