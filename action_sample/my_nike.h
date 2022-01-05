#include "api_nike.h"
#include "iostream"

namespace testing
{
class my_nike : virtual public api::nike
{
public:
    my_nike() = default;
    my_nike(int a, float b) : _a(a), _b(b)
    {
    }

    virtual ~my_nike() = default;

    virtual void do_it()
    {
        _a *= 10;
        _b *= 10.0;
        std::cout << "Inside the " << typeid(*this).name() << " class. a: " << _a << ", b: " << _b << std::endl;
    }

private:
    int _a = 1;
    float _b = 1.0;
};
}
