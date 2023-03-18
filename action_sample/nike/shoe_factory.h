#pragma once

#include "shoe.h"
#include <prgrmr/generic/factory.h>
#include <functional>
#include <memory>
#include <string>

namespace nike
{
using base_constructor = std::function<std::unique_ptr<shoe> ()>;
using numerics_constructor = std::function<std::unique_ptr<shoe> (int, float)>;

using shoe_factory =
      prgrmr::generic::key_class_factory<std::string,
                                         base_constructor,
                                         numerics_constructor>;
}
