#pragma once

#include "shoe.h"
#include <prgrmr/generic/class_name.h>
#include <iostream>

namespace nike
{
class bird : virtual public shoe
{
public:
    bird(int a, float b) : _a(a), _b(b)
    {
    }

    ~bird() override = default;

    void do_it() override
    {
        _a *= 10;
        _b *= 10.0;
        std::cout << "Inside " << prgrmr::generic::class_name(*this) << ". a: " << _a << ", b: " << _b << std::endl;
    }

private:
    int _a = 1;
    float _b = 1.0;
};
}
