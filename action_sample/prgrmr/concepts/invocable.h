#pragma once

#include "arguments.h"

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

namespace prgrmr::concepts::invocable
{

// helper trait to extract the return type of an invocable type
template<typename T>
struct invocable_result_impl;

// std::function specialization
template<typename ResultType, typename... Args>
struct invocable_result_impl<std::function<ResultType(Args...)>>
{
    using result_type = ResultType;
};

// function pointer
template<typename ResultType, typename... Args>
struct invocable_result_impl<ResultType(*)(Args...)>
{
    using result_type = ResultType;
};

// function type
template<typename ResultType, typename... Args>
struct invocable_result_impl<ResultType(Args...)>
{
    using result_type = ResultType;
};

// member function pointer (non-const)
template<typename C, typename ResultType, typename... Args>
struct invocable_result_impl<ResultType(C::*)(Args...)>
{
    using result_type = ResultType;
};

// member function pointer (const)
template<typename C, typename ResultType, typename... Args>
struct invocable_result_impl<ResultType(C::*)(Args...) const>
{
    using result_type = ResultType;
};

// member function pointer (volatile)
template<typename C, typename ResultType, typename... Args>
struct invocable_result_impl<ResultType(C::*)(Args...) volatile>
{
    using result_type = ResultType;
};

// member function pointer (const volatile)
template<typename C, typename ResultType, typename... Args>
struct invocable_result_impl<ResultType(C::*)(Args...) const volatile>
{
    using result_type = ResultType;
};

// catch-all: functor/lambda -> use &T::operator() to deduce signature
template<typename T>
struct invocable_result_impl : invocable_result_impl<decltype(&std::remove_reference_t<T>::operator())>
{};

// alias to get the return type
template<typename T>
using invocable_result_t = typename invocable_result_impl<std::decay_t<T>>::result_type;


// helper trait to extract a canonical function "signature" (return + parameter types)
// the signature_type is normalized to the plain function type R(Args...)
template<typename T>
struct invocable_signature_impl;

// std::function specialization
template<typename ResultType, typename... Args>
struct invocable_signature_impl<std::function<ResultType(Args...)>>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// function pointer
template<typename ResultType, typename... Args>
struct invocable_signature_impl<ResultType(*)(Args...)>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// function type
template<typename ResultType, typename... Args>
struct invocable_signature_impl<ResultType(Args...)>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// member function pointer (non-const)
template<typename C, typename ResultType, typename... Args>
struct invocable_signature_impl<ResultType(C::*)(Args...)>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// member function pointer (const)
template<typename C, typename ResultType, typename... Args>
struct invocable_signature_impl<ResultType(C::*)(Args...) const>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// member function pointer (volatile)
template<typename C, typename ResultType, typename... Args>
struct invocable_signature_impl<ResultType(C::*)(Args...) volatile>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// member function pointer (const volatile)
template<typename C, typename ResultType, typename... Args>
struct invocable_signature_impl<ResultType(C::*)(Args...) const volatile>
{
    using signature_type = typename std::decay_t<ResultType>(Args...);
};

// catch-all: functor/lambda -> use &T::operator() to deduce signature
template<typename T>
struct invocable_signature_impl : invocable_signature_impl<decltype(&std::remove_reference_t<T>::operator())>
{};

// normalized alias type for comparing signatures (removes cv/ref decoration)
template<typename T>
using invocable_signature_t = std::remove_cvref_t<typename invocable_signature_impl<std::decay_t<T>>::signature_type>;

///
/// Evaluate if the invocable types have the same result type.
///
template<typename ...>
inline constexpr bool is_same_result_type = false;

// single element -> trivially true
template<typename Head>
inline constexpr bool is_same_result_type<Head> = true;

// compare adjacent entries: Head vs Next, then recurse with Next and Tail...
template<typename Head, typename Next, typename ... Tail>
inline constexpr bool is_same_result_type<Head, Next, Tail...> =
    std::is_same_v<invocable_result_t<Head>, invocable_result_t<Next>> &&
    is_same_result_type<Next, Tail...>;

///
/// Concept to enforce that a list of invocable type have the same result type.
///
template<typename ... Invocables>
concept IsSameResultType = is_same_result_type<Invocables...>;

///
/// Evaluate if a list of invocables have different signatures (parameter types + return type).
/// This implements pairwise uniqueness of the canonical signature extracted by invocable_signature_t.
///
template<typename ...>
inline constexpr bool is_different_signatures = true;

// single element -> trivially true
template<typename Head>
inline constexpr bool is_different_signatures<Head> = true;

// ensure Head signature != each of Tail... and recurse for the remainder
template<typename Head, typename Next, typename ... Tail>
inline constexpr bool is_different_signatures<Head, Next, Tail...> =
    ((!std::is_same_v<invocable_signature_t<Head>, invocable_signature_t<Next>>) &&
     is_different_signatures<Next, Tail...>);

///
/// Concept to enforce that a list of invocables have different signatures (parameter types + return type).
///
template<typename ... Invocables>
concept IsDifferentSignatures = is_different_signatures<Invocables...>;

}
