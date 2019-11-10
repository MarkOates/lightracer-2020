#pragma once


#include <string>


namespace Lightracer
{
   class TimerFormatter
   {
   private:
      int milliseconds;

   public:
      TimerFormatter(int milliseconds=0);
      ~TimerFormatter();

      std::string format();

      std::string infer_hours_fragment();
      std::string infer_minutes_fragment();
      std::string infer_seconds_fragment();
      std::string infer_milliseconds_fragment();
   };
}


