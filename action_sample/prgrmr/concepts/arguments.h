#pragma once

#include "../traits/arguments.h"
#include <cstddef>

namespace prgrmr::concepts::arguments
{
///
/// Concept to check the number of template arguments is equal to the count.
///
template<std::size_t Count, typename ... Arguments>
concept IsCountEqual = traits::arguments::is_count_equal<Count, Arguments...>;

///
/// Concept to check for when there aren't any arguments.
///
template<typename ... Arguments>
concept IsEmpty = traits::arguments::is_empty<Arguments...>;

///
/// Concept to check for when there aren't any arguments.
///
template<typename ... Arguments>
concept IsNotEmpty = traits::arguments::is_not_empty<Arguments...>;

///
/// Concept to check for when there aren't any arguments.
///
template<std::size_t Count, typename ... Arguments>
concept IsAtLeast = traits::arguments::is_at_least<Count, Arguments...>;

///
/// Concept to check for when there is precisely one argument.
///
template<typename ... Arguments>
concept IsSingle = traits::arguments::is_single<Arguments...>;

///
/// Concept to check for when there are precisely a pair of arguments.
///
template<typename ... Arguments>
concept IsPair = traits::arguments::is_pair<Arguments...>;
}
