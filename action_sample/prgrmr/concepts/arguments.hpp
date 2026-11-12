#pragma once

#include <cstddef>

namespace prgrmr::concepts::arguments
{
    ///
    /// <summary>
    ///   Verifies if the templated arguments matches the specified count.
    /// </summary>
    ///
    template<std::size_t TCount, typename ... TArguments>
    concept IsCountEqual = (sizeof...(TArguments) == TCount);

    ///
    /// <summary>
    ///   Verifies if that there are no templated arguments.
    /// </summary>
    ///
    template<typename ... TArguments>
    concept IsEmpty = IsCountEqual<0, TArguments...>;

    ///
    /// <summary>
    ///   Verifies if that templated arguments is not empty.
    /// </summary>
    ///
    template<typename ... TArguments>
    concept IsNotEmpty = not IsEmpty<TArguments...>;

    ///
    /// <summary>
    ///   Verifies if that templated arguments has at least the specified count.
    /// </summary>
    ///
    template<std::size_t TCount, typename ... TArguments>
    concept IsAtLeast = (sizeof...(TArguments) >= TCount);

    ///
    /// <summary>
    ///   Verifies if that templated arguments has exactly one argument.
    /// </summary>
    ///
    template<typename ... TArguments>
    concept IsSingle = IsCountEqual<1, TArguments...>;

    ///
    /// <summary>
    ///   Verifies if that templated arguments has exactly two arguments.
    /// </summary>
    ///
    template<typename ... TArguments>
    concept IsPair = IsCountEqual<2, TArguments...>;
}
