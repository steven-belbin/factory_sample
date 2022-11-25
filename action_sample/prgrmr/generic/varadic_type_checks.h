#pragma once

#include <type_traits>

namespace prgrmr::generic
{
template <typename ... data_types> struct are_same;

template <> struct are_same<> : std::true_type {};
template <typename T> struct are_same<T> : std::true_type {};

template <typename T1, typename T2, typename... data_types>
struct are_same<T1, T2, data_types...> :
    std::conditional<
        std::is_same<T1, T2>::value,
        are_same<T2, data_types...>,
        std::false_type
    >::type
{};
}