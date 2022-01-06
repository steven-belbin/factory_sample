#pragma once

template<class T>
constexpr auto class_name()
{
    const std::string_view name = typeid(T).name();

    const auto pos = name.find_last_of(' ');

    if (pos == std::string_view::npos)
        return name;

    return name.substr(pos + 1);
}

template<class T>
constexpr auto class_name(const T&)
{
    return class_name<T>();
}
