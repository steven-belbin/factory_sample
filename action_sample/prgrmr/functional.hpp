#pragma once

#include "./concepts/invocable.hpp"
#include <functional>
#include <optional>
#include <tuple>

namespace prgrmr
{
///
/// <summary>
///  When the function is defined invokes a function with the given arguments.
///  Otherwise, doesn't invoke the function.
/// </summary>
///
/// <param name="function">The function to be invoked.</param>
/// <param name="args">The arguments to pass to the function.</param>
///
/// <returns>
///  An optional containing the result of invoking the function.
//   Otherwise, std::nullopt to indicate the function was not invoked.
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
///  When the function is defined invokes a function with the given arguments.
///  Otherwise, doesn't invoke the function.
/// </summary>
///
/// <param name="function">The function to be invoked.</param>
/// <param name="args">The arguments to pass to the function.</param>
///
/// <returns>
///  An optional containing the result of invoking the function.
//   Otherwise, std::nullopt to indicate the function was not invoked.
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
