#pragma once

#include "./concepts/invocable.hpp"
#include <functional>
#include <optional>
#include <tuple>

namespace prgrmr
{
///
/// <summary>
///  When an invocable function is defined, then applies the invocable function using the tuple of arguments.
/// </summary>
///
/// <param name="invocable">The invocable function to be applied.</param>
/// <param name="args">The tuple of arguments to pass to the function.</param>
///
/// <returns>
///  An optional containing the result of applying the function.
//   Otherwise, std::nullopt when the invocable function is not defined.
/// </returns>
///
template<class... args_t>
auto apply_optional(concepts::invocable::InvocableNullableWithResult<const args_t&...> auto&& invocable,
                    const std::tuple<args_t...>& args)
{
   return (invocable)
        ? std::make_optional(std::apply(std::forward<decltype(invocable)>(invocable), args))
        : std::nullopt;
}

///
/// <summary>
///  When an invocable function is defined, then applies the invocable function using the arguments.
/// </summary>
///
/// <param name="invocable">The invocable function to be invoked.</param>
/// <param name="args">The arguments to pass to the function.</param>
///
/// <returns>
///  An optional containing the result of invoking the function.
///  Otherwise, std::nullopt when the invocable function is not defined.
/// </returns>
///
template<class... args_t>
auto invoke_optional(concepts::invocable::InvocableNullableWithResult<const args_t&...> auto&& invocable,
                     const args_t&... args)
{
   return (invocable)
        ? std::make_optional(std::invoke(std::forward<decltype(invocable)>(invocable), args...))
        : std::nullopt;
}
}
