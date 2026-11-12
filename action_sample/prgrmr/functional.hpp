#pragma once

#include "./concepts/invocable.hpp"
#include <functional>
#include <optional>
#include <tuple>

namespace prgrmr
{
using concepts::invocable::InvocableCanBeEmptyWithResult;

///
/// <summary>
///   When the invocable is defined, then applies the invocable passing the tuple of parameters.
/// </summary>
///
/// <param name="invocable">The invocable to apply.</param>
/// <param name="parameters">The tuple of parameters to pass to the invocable.</param>
///
/// <returns>
///   An optional containing the result of applying the invocable.
///   Otherwise, std::nullopt when the invocable is empty.
/// </returns>
///
template<typename... TParameters>
auto apply_optional(InvocableCanBeEmptyWithResult auto&& invocable,
                    const std::tuple<TParameters...>& parameters)
{
   return (invocable)
        ? std::make_optional(std::apply(std::forward<decltype(invocable)>(invocable), parameters))
        : std::nullopt;
}

///
/// <summary>
///   When the invocable is defined, then applies the invocable passing the parameters.
/// </summary>
///
/// <param name="invocable">The invocable to invoke.</param>
/// <param name="parameters">The parameters to pass to the invocable.</param>
///
/// <returns>
///   An optional containing the result of invoking the invocable.
///   Otherwise, std::nullopt when the invocable is empty.
/// </returns>
///
template<typename... TParameters>
auto invoke_optional(InvocableCanBeEmptyWithResult auto&& invocable,
                     const TParameters&... parameters)
{
   return (invocable)
        ? std::make_optional(std::invoke(std::forward<decltype(invocable)>(invocable), parameters...))
        : std::nullopt;
}
}
