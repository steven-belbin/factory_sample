#pragma once

#include "arguments.h"

#include <concepts>
#include <type_traits>

namespace prgrmr::concepts::invocable
{

///
/// Concept verifying that two invocable functions return the same return type.
///
template<typename LHS, typename RHS>
concept IsSameReturnType = std::invocable<LHS>
                        && std::invocable<RHS>
                        && std::is_same_v<typename std::invoke_result<LHS>::type,
                                          typename std::invoke_result<RHS>::type>;

///
/// Expression to indicate if the given sequence of arguments represents a
/// sequence invocable functions ALL of which return the same type.
///
template<typename LHS, typename RHS>
requires (std::invocable<LHS> && std::invocable<RHS>)
inline constexpr bool are_same_return_type = std::is_same_v<typename std::invoke_result<LHS>::type,
                                                            typename std::invoke_result<RHS>::type>;

///
/// Expression to indicate if the given sequence of arguments represents a
/// sequence invocable functions ALL of which return the same type.
///
template<typename Head, typename...Tail>
inline constexpr bool are_all_same_return_type = (are_same_return_type<Head, Tail> && ...);

///
/// Concept to verify if the given sequence of arguments represents a
/// sequence invocable functions ALL of which return the same type.
///
template<typename Head, typename ...Tail>
concept AreAllSameReturnType = are_all_same_return_type<Head, Tail...>;

///
/// Concept verifying that a pair of invocable functions are considered as being the same.
///
template<typename LHS, typename RHS>
concept IsSame = std::invocable<LHS>
              && std::invocable<RHS>
              && std::is_same_v<LHS, RHS>;

///
/// Concept verifying that a pair of invocable functions are considered as being the different.
///
template<typename LHS, typename RHS>
concept IsDifferent = !IsSame<LHS, RHS>;

///
/// Expression that indicates that the given arguments represents a sequence of invocable functions that are all
/// different from each other.
///
/// This is the default implementation for when there one argument or none.
///
template <typename...Functions>
inline constexpr bool are_all_different = arguments::IsEmpty<Functions>
                                       || (arguments::IsSingle<Functions> && std::invocable<Functions>);

///
/// Expression that indicates that the given arguments represents a sequence of invocable functions that are all
/// different from each other.
///
template <typename Head, typename ... Tail>
inline constexpr bool are_all_different<Head, Tail...> = IsDifferent<Head, Tail...>
                                                      && are_all_different<Tail...>;

///
/// Concept verifying that the given arguments represents a sequence of invocable functions that are all different from each other.
///
template<typename Functions>
concept AreAllDifferent = are_all_different<Functions>;

template<typename ... Functions>
    requires (std::invocable<Functions> && ...)
inline constexpr bool are_all_invocable = true;

template<typename Functions>
concept AreAllInvocable = are_all_invocable<Functions>;

/////
///// Expression to check if the given pair of invocable functions are the same.
///// 
//template <typename LHS, typename RHS>
//inline constexpr bool is_pair_unique_invocable = std::invocable<LHS>
//                                              && std::invocable<RHS>
//                                              && !std::is_same_v<LHS, RHS>;

//template<typename ... head_t>
//concept AreAllInvocable = AreAllInvocable<head_t...>;

}
