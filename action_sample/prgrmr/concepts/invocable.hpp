#pragma once

#include <concepts>
#include <functional>
#include <tuple>
#include <type_traits>

namespace prgrmr::concepts::invocable
{
    namespace detail
    {
        ///
        /// <summary>
        ///   Extracts the invocable's result type, parameters and signature.
        /// </summary>
        ///
        template<typename Invocable>
        struct invocable_traits;

        ///
        /// <summary>
        ///   Extracts the invocable's result type, parameters and signature.
        /// </summary>
        ///
        template<typename Result, typename... Parameters>
        struct invocable_traits<Result(Parameters...)>
        {
            using result_type = Result;
            using parameters_type = std::tuple<Parameters...>;
            using signature_type = Result(Parameters...);
        };

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a function pointer.
        /// </summary>
        ///
        template<typename Result, typename... Parameters>
        struct invocable_traits<Result(*)(Parameters...)> : invocable_traits<Result(Parameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a std::function.
        /// </summary>
        ///
        template<typename Result, typename... Parameters>
        struct invocable_traits<std::function<Result(Parameters...)>> : invocable_traits<Result(Parameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a non-const member function pointer.
        /// </summary>
        ///
        template<typename Class, typename Result, typename... Parameters>
        struct invocable_traits<Result(Class::*)(Parameters...)> : invocable_traits<Result(Parameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a const member function pointer.
        /// </summary>
        ///
        template<typename Class, typename Result, typename... Parameters>
        struct invocable_traits<Result(Class::*)(Parameters...) const> : invocable_traits<Result(Parameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a functor or lambda by forwarding to its operator().
        /// </summary>
        ///
        template<typename Class>
        requires requires { &std::remove_reference_t<Class>::operator(); }
        struct invocable_traits<Class> : invocable_traits<decltype(&std::remove_reference_t<Class>::operator())> {};

        template<typename Invocable>
        using invocable_result_type = typename invocable_traits<std::decay_t<Invocable>>::result_type;

        template<typename Invocable>
        using invocable_parameters_type = typename invocable_traits<std::decay_t<Invocable>>::parameters_type;

        template<typename Invocable>
        using invocable_signature_type = typename invocable_traits<std::decay_t<Invocable>>::signature_type;

        ///
        /// <summary>
        ///   Checks that all signatures are unique.
        /// </summary>
        ///
        template<typename... Signatures>
        consteval bool are_invocables_signatures_unique()
        {
            if constexpr (sizeof...(Signatures) <= 1)
            {
                return true;
            }
            else
            {
                return []<typename Signature, typename... RemainingSignatures>(std::type_identity<Signature>, std::type_identity<RemainingSignatures>...)
                {
                    if constexpr ((std::is_same_v<Signature, RemainingSignatures> || ...))
                    {
                        return false;
                    }
                    else
                    {
                        return are_invocables_signatures_unique<RemainingSignatures...>();
                    }
                }(std::type_identity<Signatures>{}...);
            }
        }

        template<typename... Invocables, typename... Parameters>
        consteval std::size_t find_invocable_index(std::tuple<Parameters...>* = nullptr)
        {
            constexpr std::size_t match_count = ((std::is_invocable_v<Invocables, Parameters...> ? 1 : 0) + ...);

            static_assert(match_count > 0, "No matching function found for the provided arguments.");
            static_assert(match_count <= 1, "Ambiguous call: multiple matching functions found for the provided arguments.");

            constexpr bool matches[] = { std::is_invocable_v<Invocables, Parameters...>... };
            constexpr std::size_t count = sizeof...(Invocables);

            for (std::size_t index = 0; index < count; ++index)
            {
                if (matches[index])
                {
                    return index;
                }
            }

            return 0;
        }

        template<typename Parameters, typename... Invocables>
        struct find_invocable;

        template<typename... Parameters, typename... Invocables>
        struct find_invocable<std::tuple<Parameters...>, Invocables...>
        {
            using type = std::tuple_element_t<
                find_invocable_index<Invocables...>(static_cast<std::tuple<Parameters...>*>(nullptr)),
                std::tuple<Invocables...>>;
        };
    }

    ///
    /// <summary>
    ///   Deduce the invocable based on its parameters from the series of invocables.
    /// </summary>
    ///
    template<typename Parameters, typename... Invocables>
    using find_invocable_type = typename detail::find_invocable<Parameters, Invocables...>::type;

    ///
    /// <summary>
    ///   Deduce result type of an invocable.
    /// </summary>
    ///
    template<typename Invocable>
    using invocable_result_type = detail::invocable_result_type<Invocable>;

    ///
    /// <summary>
    ///   Deduce an invocable's signature which is Result(Parameters...)
    /// </summary>
    template<typename Invocable>
    using invocable_signature_type = detail::invocable_signature_type<Invocable>;

    ///
    /// <summary>
    ///   A series of invocables.
    /// </summary>
    ///
    template<typename... Invocables>
    using invocables_type = std::tuple<std::decay_t<Invocables>...>;

    ///
    /// <summary>
    ///   The invocables common result type (taken from the first).
    /// </summary>
    ///
    template<typename... Invocables>
    requires (sizeof...(Invocables) > 0)
    using invocables_result_type = invocable_result_type<std::tuple_element_t<0, std::tuple<Invocables...>>>;

    ///
    /// <summary>
    ///   Verify that all invocables in the series have the same result type.
    /// </summary>
    ///
    template<typename... Invocables>
    concept IsSameResultType = (sizeof...(Invocables) <= 1) ||
        []<typename Invocable, typename... RemaingInvocables>(std::type_identity<Invocable>, std::type_identity<RemaingInvocables>...)
        {
            return (std::is_same_v<invocable_result_type<Invocable>, invocable_result_type<RemaingInvocables>> && ...);
        }(std::type_identity<Invocables>{}...);

    ///
    /// <summary>
    ///   Verify that each invocable in the series has a unique signature -> Result(Parameters...).
    /// </summary>
    ///
    template<typename... Invocables>
    concept AreInvocablesSignaturesUnique = detail::are_invocables_signatures_unique<detail::invocable_signature_type<Invocables>...>();

    ///
    ///
    /// <summary>
    ///   Verify that each invocable in the series has different parameters.
    /// </summary>
    ///
    template<typename Parameters, typename... Invocables>
    struct are_invocables_parameters_unique : std::false_type {};

    template<typename... Parameters, typename... Invocables>
    struct are_invocables_parameters_unique<std::tuple<Parameters...>, Invocables...>
         : std::bool_constant<((std::is_invocable_v<Invocables, Parameters...> ? 1 : 0) + ...) == 1> {};

    template<typename Parameters, typename... Invocables>
    concept AreInvocablesParametersUnique = are_invocables_parameters_unique<Parameters, Invocables...>::value;

    ///
    /// <summary>
    ///     Concept of an invocable that supports nullability.
    /// </summary>
    ///
    template<typename InvocableT, typename... Parameters>
    concept Invocable =
        std::invocable<InvocableT, Parameters...>;

    ///
    /// <summary>
    ///     Concept of an invocable that supports nullability.
    /// </summary>
    ///
    template<typename InvocableT, typename... Parameters>
    concept InvocableNullable =
        Invocable<InvocableT, Parameters...> &&
        requires(const std::remove_reference_t<InvocableT>& invocable)
        {
            static_cast<bool>(invocable);
        };

    ///
    /// <summary>
    ///     Concept of invocable with a void result.
    /// </summary>
    ///
    template<typename InvocableT, typename... Parameters>
    concept InvocableVoidResult =
        Invocable<InvocableT, Parameters...> &&
        std::is_void_v<invocable_result_type<InvocableT>>;

    ///
    /// <summary>
    ///     Concept of invocable with a result (non-void).
    /// </summary>
    ///
    template<typename InvocableT, typename... Parameters>
    concept InvocableWithResult =
        Invocable<InvocableT, Parameters...> &&
        !InvocableVoidResult<InvocableT, Parameters...>;

    ///
    /// <summary>
    ///     Concept of an invocable that is both nullable with a result (non-void).
    /// </summary>
    ///
    template<typename Invocable, typename... Parameters>
    concept InvocableNullableWithResult =
        InvocableNullable<Invocable, Parameters...> &&
        InvocableWithResult<Invocable, Parameters...>;
}
