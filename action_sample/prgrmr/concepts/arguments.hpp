#pragma once

#include <cstddef>

namespace prgrmr::concepts::arguments
{
    ///
    /// Concept to check the number of template arguments is equal to the count.
    ///
    template<std::size_t Count, typename ... Arguments>
    concept IsCountEqual = (sizeof...(Arguments) == Count);

    ///
    /// Concept to check for when there aren't any arguments.
    ///
    template<typename ... Arguments>
    concept IsEmpty = IsCountEqual<0, Arguments...>;

    ///
    /// Concept to check for when there is at least one argument.
    ///
    template<typename ... Arguments>
    concept IsNotEmpty = not IsEmpty<Arguments...>;

    ///
    /// Concept to check that the number of template arguments is at least the count.
    ///
    template<std::size_t Count, typename ... Arguments>
    concept IsAtLeast = (sizeof...(Arguments) >= Count);

    ///
    /// Concept to check for when there is precisely one argument.
    ///
    template<typename ... Arguments>
    concept IsSingle = IsCountEqual<1, Arguments...>;

    ///
    /// Concept to check for when there are precisely a pair of arguments.
    ///
    template<typename ... Arguments>
    concept IsPair = IsCountEqual<2, Arguments...>;
}
