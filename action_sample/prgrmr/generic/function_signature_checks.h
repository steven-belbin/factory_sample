#pragma once

#include <type_traits>

namespace prgrmr::generic
{
template <typename ... functions_t> struct are_invoke_result_same;

template <> struct are_invoke_result_same<> : std::true_type {};
template <typename function_t> struct are_invoke_result_same<function_t> : std::true_type {};

template <typename function_1_t, typename function_2_t, typename... functions_t>
struct are_invoke_result_same<function_1_t, function_2_t, functions_t...> :
    std::conditional<
      std::is_same<function_1_t, function_2_t>::value,
      are_invoke_result_same<function_2_t, functions_t...>,
      std::false_type
    >::type
{};

}