#pragma once

#include <string_view>
#include <typeinfo>

namespace prgrmr
{
template<class TClass>
constexpr auto class_name()
{
    const std::string_view name = typeid(TClass).name();

    const auto pos = name.find_last_of(' ');

    if (pos == std::string_view::npos)
        return name;

    return name.substr(pos + 1);
}

template<class TClass>
constexpr auto class_name(const TClass&)
{
    return class_name<TClass>();
}

}