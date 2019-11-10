#pragma once


#include "Lightracer/Banner/Base.hpp"


namespace Banner
{
   class TrackCompleted : public Banner::Base
   {
   public:
      TrackCompleted();
      ~TrackCompleted();

      void draw() override;
   };
}


