#pragma once

#ifndef __EVENT_OBJECT_HEADER
#define __EVENT_OBJECT_HEADER


#include <algorithm>
#include <vector>


class Framework;



class uses_keyboard
{
private:
   static std::vector<uses_keyboard *> obj;

   static void update_all_on_key_up();
   static void update_all_on_key_down();
   static void update_all_on_key_char();

   friend Framework;

public:
   virtual void on_key_char() {}
   virtual void on_key_up() {}
   virtual void on_key_down() {}

   uses_keyboard();
   ~uses_keyboard();
};




#endif
