#pragma once

#include "factory.h"

#include <functional>
#include <string>

namespace api
{
class nike
{
public:
   nike() = default;
   virtual ~nike() = 0 {};
   virtual void do_it() = 0;

private:
   nike(const nike&) = delete;
   nike& operator=(const nike&) = delete;
};

typedef prgrmr::generic::key_class_factory<std::string,
                                           std::function<nike* ()>,
                                           std::function<nike* (int, float)>> nike_factory;
}
