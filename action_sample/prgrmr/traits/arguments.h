#pragma once

#include <cstddef>

namespace prgrmr::traits::arguments
{
template<std::size_t Count, typename ... Arguments>
inline constexpr bool is_count_equal = sizeof...(Arguments) == Count;

template<typename ... Arguments>
inline constexpr bool is_empty = is_count_equal<0, Arguments...>;

template<typename ... Arguments>
inline constexpr bool is_not_empty = !is_empty<Arguments...>;

template<std::size_t Count, typename ... Arguments>
inline constexpr bool is_at_least = sizeof...(Arguments) >= Count;

template<typename ... Arguments>
concept is_single = is_count_equal<1, Arguments...>;

template<typename ... Arguments>
concept is_pair = is_count_equal<2, Arguments...>;
}
