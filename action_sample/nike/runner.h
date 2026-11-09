#pragma once

#include "shoe.h"
#include <prgrmr/class_name.hpp>
#include <iostream>

namespace nike
{
class runner : virtual public shoe
{
public:
    runner() = default;
    ~runner() override = default;

    void do_it() override
    {
        std::cout << "Inside " << prgrmr::class_name(*this) << "." << std::endl;
    }
};
}
