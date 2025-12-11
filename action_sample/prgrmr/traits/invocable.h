#pragma once

#include "arguments.h"

#include <functional>
#include <type_traits>
#include <utility>

namespace prgrmr::traits::invocable
{

///
/// Helper trait to extract the result & signature types.
///
template<typename Result, typename ...Arguments>
struct invocable_detail
{
    using result_type = Result;
    using signature_type = Result(Arguments...);
};

template<typename Invocable>
struct invocable;

///
/// std::function specialization
///
template<typename Result, typename... Arguments>
struct invocable<std::function<Result(Arguments...)>> : invocable_detail<Result, Arguments...> {};

///
/// function pointer
///
template<typename Result, typename... Arguments>
struct invocable<Result(*)(Arguments...)> : invocable_detail<Result, Arguments...> {};

///
/// function type
///
template<typename Result, typename... Arguments>
struct invocable<Result(Arguments...)> : invocable_detail<Result, Arguments...> {};

///
/// member function pointer (non-const)
///
template<typename C, typename Result, typename... Arguments>
struct invocable<Result(C::*)(Arguments...)> : invocable_detail<Result, Arguments...> {};

///
/// member function pointer (const)
///
template<typename C, typename Result, typename... Arguments>
struct invocable<Result(C::*)(Arguments...) const> : invocable_detail<Result, Arguments...> {};

///
/// member function pointer (volatile)
///
template<typename C, typename Result, typename... Arguments>
struct invocable<Result(C::*)(Arguments...) volatile> : invocable_detail<Result, Arguments...> {};

///
/// member function pointer (const volatile)
///
template<typename C, typename Result, typename... Arguments>
struct invocable<Result(C::*)(Arguments...) const volatile> : invocable_detail<Result, Arguments...> {};

///
/// Catch all: functor/lambda -> use &T::operator() to deduce signature
///
template<typename Invocable>
struct invocable : invocable<decltype(&std::remove_reference_t<Invocable>::operator())> {};

///
/// Get an invocable's result type.
///
template<typename Invocable>
using invocable_result_t = typename invocable<std::decay_t<Invocable>>::result_type;

///
/// Get an invocable's signature type.
///
template<typename Invocable>
using invocable_signature_t = typename invocable<std::decay_t<Invocable>>::signature_type;

///
/// Evaluate if a list invocables have the same result type.
///
template<typename ...>
inline constexpr bool is_same_result_type = false;

///
/// Single invocable is trivially true.
///
template<typename Invocable>
inline constexpr bool is_same_result_type<Invocable> = true;

///
/// Evaluates that each invocable's result is the same as the next one in the list.
///
template<typename Head, typename Next, typename ... Tail>
inline constexpr bool is_same_result_type<Head, Next, Tail...> =
    std::is_same_v<invocable_result_t<Head>, invocable_result_t<Next>> &&
    is_same_result_type<Next, Tail...>;

///
/// Evaluate if a list of invocables signatures are unique (parameter types & result type).
///
template<typename ...>
inline constexpr bool is_different_signatures = true;

///
/// A single invocable is trivially true.
///
template<typename Invocable>
inline constexpr bool is_different_signatures<Invocable> = true;

///
/// Evaluates that each invocable's signature is unique within the list.
///
/// <todo>
/// Check the head against all next invocables, not just the next one.
/// </todo>
///
template<typename Head, typename Next, typename ... Tail>
inline constexpr bool is_different_signatures<Head, Next, Tail...> =
    ((!std::is_same_v<invocable_signature_t<Head>, invocable_signature_t<Next>>) &&
     is_different_signatures<Next, Tail...>);
}
