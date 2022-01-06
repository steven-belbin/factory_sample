#pragma once

#include "api_nike.h"
#include "class_name.h"
#include "iostream"

namespace testing
{
class your_nike : virtual public api::nike
{
public:
    your_nike() = default;
    your_nike(int a, float b) : _a(a), _b(b)
    {
    }

    virtual ~your_nike() = default;

    virtual void do_it()
    {
        _a *= 2;
        _b *= 2.2f;
        std::cout << "Inside " << class_name(*this) << ". a: " << _a << ", b : " << _b << std::endl;
    }

private:
    int _a = 77;
    float _b = 77.7f;
};
}
