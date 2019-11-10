#pragma once


namespace Banner
{
   class Base
   {
   public:
      Base();
      virtual ~Base();

      virtual void draw() = 0;
   };
}


