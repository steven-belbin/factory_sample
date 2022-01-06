#pragma once

#include "shoe.h"
#include <prgrmr/generic/class_name.h>
#include <iostream>

namespace nike
{
class jordan : virtual public shoe
{
public:
    jordan() = default;
    jordan(int a, float b) : _a(a), _b(b)
    {
    }

    ~jordan() override = default;

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
