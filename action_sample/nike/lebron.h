#pragma once

#include "shoe.h"
#include <prgrmr/generic/class_name.h>
#include <iostream>

namespace nike
{
class lebron : virtual public shoe
{
public:
    lebron() = default;
    lebron(int a, float b) : _a(a), _b(b)
    {
    }

    ~lebron() override = default;

    void do_it() override
    {
        _a *= 2;
        _b *= 2.2f;
        std::cout << "Inside " << prgrmr::generic::class_name(*this) << ". a: " << _a << ", b : " << _b << std::endl;
    }

private:
    int _a = 77;
    float _b = 77.7f;
};
}
