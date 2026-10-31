#pragma once

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>

namespace prgrmr::concepts::invocable
{
    namespace detail
    {
        // Extracts the signature and return type of an invocable type
        template<typename Function>
        struct function_traits;

        // Function type
        template<typename Return, typename... Parameters>
        struct function_traits<Return(Parameters...)>
        {
            using return_type = Return;
            using signature_type = Return(Parameters...);
        };

        // Function pointer
        template<typename Return, typename... Parameters>
        struct function_traits<Return(*)(Parameters...)> : function_traits<Return(Parameters...)> {};

        // std::function
        template<typename Return, typename... Parameters>
        struct function_traits<std::function<Return(Parameters...)>> : function_traits<Return(Parameters...)> {};

        // Member function pointer (non-const)
        template<typename Class, typename Return, typename... Parameters>
        struct function_traits<Return(Class::*)(Parameters...)> : function_traits<Return(Parameters...)> {};

        // Member function pointer (const)
        template<typename Class, typename Return, typename... Parameters>
        struct function_traits<Return(Class::*)(Parameters...) const> : function_traits<Return(Parameters...)> {};

        // Functor / lambda: forward to operator()
        template<typename T>
        requires requires { &std::remove_reference_t<T>::operator(); }
        struct function_traits<T> : function_traits<decltype(&std::remove_reference_t<T>::operator())> {};

        template<typename Function>
        using invoke_return_t = typename function_traits<std::decay_t<Function>>::return_type;

        template<typename Function>
        using invoke_signature_t = typename function_traits<std::decay_t<Function>>::signature_type;

        ///
        /// Checks that all signatures within a pack are unique.
        ///
        template<typename... Signatures>
        consteval bool are_unique_signatures()
        {
            if constexpr (sizeof...(Signatures) <= 1)
            {
                return true;
            }
            else
            {
                return []<typename T, typename... Rest>(std::type_identity<T>, std::type_identity<Rest>...)
                {
                    if constexpr ((std::is_same_v<T, Rest> || ...))
                    {
                        return false;
                    }
                    else
                    {
                        return are_unique_signatures<Rest...>();
                    }
                }(std::type_identity<Signatures>{}...);
            }
        }

        template<typename... Invocables, typename... Args>
        consteval std::size_t find_invocable_index(std::tuple<Args...>* = nullptr)
        {
            constexpr bool matches[] = { std::is_invocable_v<Invocables, Args...>... };
            constexpr std::size_t total = sizeof...(Invocables);

            for (std::size_t i = 0; i < total; ++i)
            {
                if (matches[i])
                {
                    return i;
                }
            }

            return total;
        }

        template<typename ArgsTuple, typename... Invocables>
        struct find_invocable;

        template<typename... Args, typename... Invocables>
        struct find_invocable<std::tuple<Args...>, Invocables...>
        {
            using type = std::tuple_element_t<
                find_invocable_index<Invocables...>(static_cast<std::tuple<Args...>*>(nullptr)),
                std::tuple<Invocables...>>;
        };
    }

    ///
    /// Deduce the invocable type from a pack that matches the given argument types tuple.
    ///
    template<typename ArgsTuple, typename... Invocables>
    using find_invocable_t = typename detail::find_invocable<ArgsTuple, Invocables...>::type;

    ///
    /// Deduce return type of an invocable.
    ///
    template<typename Function>
    using invocable_return_t = detail::invoke_return_t<Function>;

    ///
    /// Deduce canonical signature type of an invocable: Return(Parameters...)
    ///
    template<typename Function>
    using invocable_signature_t = detail::invoke_signature_t<Function>;

    ///
    /// Tuple of decayed invocable types.
    ///
    template<typename... Invocables>
    using invocables_tuple_t = std::tuple<std::decay_t<Invocables>...>;

    ///
    /// Common result type of the invocables pack (taken from the first).
    ///
    template<typename... Invocables>
    requires (sizeof...(Invocables) > 0)
    using invocables_result_t = invocable_return_t<std::tuple_element_t<0, std::tuple<Invocables...>>>;

    ///
    /// Concept to enforce that all invocables in the pack return the same type.
    ///
    template<typename... Invocables>
    concept IsSameResultType = (sizeof...(Invocables) <= 1) ||
        []<typename First, typename... Rest>(std::type_identity<First>, std::type_identity<Rest>...)
        {
            return (std::is_same_v<detail::invoke_return_t<First>, detail::invoke_return_t<Rest>> && ...);
        }(std::type_identity<Invocables>{}...);

    template<typename... Invocables>
    concept IsSameReturn = IsSameResultType<Invocables...>;

    ///
    /// Concept to enforce that all invocables have distinct canonical signatures (parameters + return).
    ///
    template<typename... Invocables>
    concept IsDifferentSignatures = detail::are_unique_signatures<detail::invoke_signature_t<Invocables>...>();

    ///
    /// Concept to enforce that exactly one invocable matches the given argument types.
    ///
    template<typename ArgsTuple, typename... Invocables>
    struct is_unique_invocable : std::false_type {};

    template<typename... Args, typename... Invocables>
    struct is_unique_invocable<std::tuple<Args...>, Invocables...>
        : std::bool_constant<((std::is_invocable_v<Invocables, Args...> ? 1 : 0) + ...) == 1> {};

    template<typename ArgsTuple, typename... Invocables>
    concept IsUniqueInvocable = is_unique_invocable<ArgsTuple, Invocables...>::value;
}
