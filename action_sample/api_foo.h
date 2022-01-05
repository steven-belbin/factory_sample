#pragma once

#include "factory.h"

#include <functional>
#include <string>

namespace api
{
class foo
{
public:
   foo() = default;
   virtual ~foo() = 0 {};
   virtual void do_it() = 0;

private:
   foo(const foo&) = delete;
   foo& operator=(const foo&) = delete;
};

typedef prgrmr::generic::key_class_factory<std::string,
                                           std::function<foo* ()>,
                                           std::function<foo* (int, float)>> foo_factory;
}
