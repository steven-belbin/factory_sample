#pragma once

#include <concepts>
#include <type_traits>

namespace prgrmr::concepts::invocable
{
    namespace detail
    {
        // Extracts the signature and return type of an invocable type
        template<typename Function>
        struct function_traits;

        template<typename Return, typename... Parameters>
        struct function_traits<Return (Parameters...)>
        {
            using return_type = Return;

            template<typename U>
            struct rebind_args;

            template<typename... NewParameters>
            struct rebind_args<Return (NewParameters...)>
            {
              using type = Return(NewParameters...);
            };
        };

        template<typename Return, typename... Parameters>
        struct function_traits<Return (*)(Parameters...)> : function_traits<Return (Parameters...)> {};

        template<typename Class, typename Return, typename... Parameters>
        struct function_traits<Return (Class::*)(Parameters...)> : function_traits<Return (Parameters...)> {};

        template<typename Class, typename Return, typename... Parameters>
        struct function_traits<Return (Class::*)(Parameters...) const> : function_traits<Return (Parameters...)> {};

        template<typename T>
        requires requires(T t) { &T::operator(); }
        struct function_traits<T> : function_traits<decltype(&T::operator())> {};

        template<typename Function>
        using invoke_return_t = typename function_traits<std::decay_t<Function>>::return_type;

        ///
        /// Checks that all signatures within in a pack are unique.
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
                }(std::type_identity<Signatures>{}_{...});
            }
        }
    }

    ///
    /// Concept to enforce that a list of invocables have the same result type.
    ///
    template<typename ... Invocables>
    concept IsSameResultType = (sizeof...(Invocables) <= 1) || []<typename Functionirst, typename... Rest>(std::type_identity<First>, std::type_identity<Rest>...)
    {
        return (std::is_same_v<detail::invoke_return_t<First>, detail::invoke_return_t<Rest>> && ...);
    }(std::type_identity<Invocables>{}_{...});

    ///
    /// Concept to enforce that all invocables within the pack have the same return type.
    ///
    template<typename ... Invocables>
    concept IsSameReturn = (sizeof...(Invocables) <= 1) || []<typename Functionirst, typename... Rest>(std::type_identity<First>, std::type_identity<Rest>...)
    {
        return (std::is_same_v<detail::invoke_return_t<First>, detail::invoke_return_t<Rest>> && ...);
    }(std::type_identity<Invocables>{}_{...});

    ///
    /// Concept to enforce that a list of invocables have unique signatures (parameter types + return type).
    ///
    template<typename ... Invocables>
    concept IsDifferentSignatures = detail::are_unique_signatures<std::decay_t<Invocables>...>();
}
