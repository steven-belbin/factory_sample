#pragma once

#include <cstddef>

namespace prgrmr::concepts::arguments
{
template<std::size_t Count, typename ... Arguments>
inline constexpr bool is_count_equal = sizeof...(Arguments) == Count;

///
/// Mnemonic concept to check the number of template arguments is equal to the count.
///
template<std::size_t Count, typename ... Arguments>
concept IsCountEqual = sizeof...(Arguments) == Count;

template<std::size_t Count, typename ... Arguments>
inline constexpr bool is_empty = sizeof...(Arguments) == 0;

///
/// Mnemonic concept to check for when there aren't any arguments.
///
template<typename ... Arguments>
concept IsEmpty = IsCountEqual<0, Arguments...>;

template<typename ... Arguments>
inline constexpr bool is_not_empty = sizeof...(Arguments) > 0;

///
/// Mnemonic concept to check for when there aren't any arguments.
///
template<typename ... Arguments>
concept IsNotEmpty = !IsEmpty<Arguments...>;

///
/// Mnemonic concept to check for when there aren't any arguments.
///
template<std::size_t Count, typename ... Arguments>
concept AtLeast = sizeof...(Arguments) >= Count;

///
/// Mnemonic concept to check for when there is precisely one argument.
///
template<typename ... Arguments>
concept IsSingle = IsCountEqual<1, Arguments...>;

///
/// Mnemonic concept to check for when there are precisely a pair of arguments.
///
template<typename ... Arguments>
concept IsPair = IsCountEqual<2, Arguments...>;
}
