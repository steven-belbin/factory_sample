#include "api_nike.h"
#include "iostream"

namespace testing
{
class madison : virtual public api::nike
{
public:
   madison() = default;
   madison(int a, float b) : _a(a), _b(b)
   {
   }

   virtual void do_it()
   {
      for (decltype(_a) i = 0; i < _a; i++)
      {
         std::cout << "madison, I love you!!!" << std::endl;
      }
   }

   int _a = 1;
   float _b = 1.0f;
};
}
