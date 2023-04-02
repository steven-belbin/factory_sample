#pragma once

#include "arguments.h"

#include <concepts>
#include <type_traits>

namespace prgrmr::concepts::invocable
{
///
/// Expression that evaluates that there all arguments are invocable functions.
///
/// There is a requirement that there must be at least one arguement.
///
template<typename Function>
concept Invocable = std::invocable<Function>;

///
/// Expression that evaluates that there all arguments are invocable functions.
///
/// There is a requirement that there must be at least one arguement.
///
template<typename ... Functions>
inline constexpr bool are_all_invocable = ((arguments::IsNotEmpty<Functions...> && Invocable<Functions>) && ...);

///
/// Concept to enforce that all arguments are invocable functions.
///
template<typename ... Functions>
concept AreAllInvocable = are_all_invocable<Functions...>;

///
/// Expression that evaluates if a pair of invocable functions' return types are the same type.
///
template<typename LHS, typename RHS>
inline constexpr bool is_same_return_type = Invocable<LHS>
                                         && Invocable<RHS>
                                         && std::is_same_v<typename std::invoke_result_t<LHS>,
                                                           typename std::invoke_result_t<RHS>>;

///
/// Concept to enforce that two invocable functions' return types shall be of the same type.
///
template<typename LHS, typename RHS>
concept IsSameReturnType = is_same_return_type<LHS, RHS>;

///
/// Expression that evaluates if all the invocable functions' return types are of the same type.
///
template<typename ... Functions>
inline constexpr bool are_all_same_return_type = AreAllInvocable<Functions...>;

///
/// Expression that evaluates if all the invocable functions' return types are of the same type.
///
template<typename Head, typename...Tail>
inline constexpr bool are_all_same_return_type<Head, Tail...> = (is_same_return_type<Head, Tail> && ...);

///
/// Concept to enforce that all invocable functions' return types shall be of the same type.
///
template<typename ... Functions>
concept AreAllSameReturnType = are_all_same_return_type<Functions...>;

///
/// Expression that evaluates if a pair of invocable functions are the same.
///
template<typename LHS, typename RHS>
inline constexpr bool is_same = Invocable<LHS> && Invocable<RHS> && std::is_same_v<LHS, RHS>;

///
/// Expression that evaluates if a pair of invocable functions are the different.
///
template<typename LHS, typename RHS>
inline constexpr bool is_different = !is_same<LHS, RHS>;

///
/// Concept to enforce that a pair of invocable functions are to be different.
///
template<typename LHS, typename RHS>
concept IsDifferent = is_different<LHS, RHS>;

///
/// Expression that evaluates if all invocable functions are the same.
///
template<typename ... Functions>
inline constexpr bool are_all_different = AreAllInvocable<Functions...>;

///
/// Expression that evaluates if all the invocable functions are same type.
///
template<typename Head, typename...Tail>
inline constexpr bool are_all_different<Head, Tail...> = (is_different<Head, Tail> && ...)
                                                      && are_all_different<Tail...>;

///
/// Concept to enforce that all invocable functions shall be different.
///
template<typename ... Functions>
concept AreAllDifferent = are_all_different<Functions...>;

}
