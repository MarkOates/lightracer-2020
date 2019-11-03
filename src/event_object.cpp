
#include "event_object.h"





// uses_keyboard //

void uses_keyboard::update_all_on_key_up()
{
   for (int i=0; i<(int)obj.size(); i++) obj[i]->on_key_up();
}

void uses_keyboard::update_all_on_key_down()
{
   for (int i=0; i<(int)obj.size(); i++) obj[i]->on_key_down();
}

void uses_keyboard::update_all_on_key_char()
{
   for (int i=0; i<(int)obj.size(); i++) obj[i]->on_key_char();
}

uses_keyboard::uses_keyboard()
{
   obj.push_back(this);
}

uses_keyboard::~uses_keyboard()
{
   std::vector<uses_keyboard *>::iterator it = find(obj.begin(), obj.end(), this);
   if (it != obj.end()) { delete (*it); obj.erase(it); }
}

std::vector<uses_keyboard *> uses_keyboard::obj;




