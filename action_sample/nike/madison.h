#pragma once

#include "shoe.h"
#include <iostream>

namespace nike
{
class madison : virtual public shoe
{
public:
   madison() = default;
   madison(int a, float b) : _a(a), _b(b)
   {
   }

   ~madison() override = default;

   void do_it() override
   {
      for (decltype(_a) i = 0; i < _a; i++)
      {
         std::cout << "Madison, I love you " << _b * 1000.0f << "!!!" << std::endl;
      }
   }

   int _a = 3;
   float _b = 3.0f;
};
}
