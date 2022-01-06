#pragma once

#include "shoe.h"
#include <prgrmr/generic/class_name.h>
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
        std::cout << "Inside " << prgrmr::generic::class_name(*this) << "." << std::endl;
    }
};
}
