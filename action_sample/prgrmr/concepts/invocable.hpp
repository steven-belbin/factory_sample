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
        template<typename TInvocable>
        struct invocable_traits;

        ///
        /// <summary>
        ///   Extracts the invocable's result type, parameters and signature.
        /// </summary>
        ///
        template<typename TResult, typename... TParameters>
        struct invocable_traits<TResult(TParameters...)>
        {
            using result_type = TResult;
            using parameters_type = std::tuple<TParameters...>;
            using signature_type = TResult(TParameters...);
        };

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a function pointer.
        /// </summary>
        ///
        template<typename TResult, typename... TParameters>
        struct invocable_traits<TResult(*)(TParameters...)> : invocable_traits<TResult(TParameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a std::function.
        /// </summary>
        ///
        template<typename TResult, typename... TParameters>
        struct invocable_traits<std::function<TResult(TParameters...)>> : invocable_traits<TResult(TParameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a non-const member function pointer.
        /// </summary>
        ///
        template<typename TClass, typename TResult, typename... TParameters>
        struct invocable_traits<TResult(TClass::*)(TParameters...)> : invocable_traits<TResult(TParameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a const member function pointer.
        /// </summary>
        ///
        template<typename TClass, typename TResult, typename... TParameters>
        struct invocable_traits<TResult(TClass::*)(TParameters...) const> : invocable_traits<TResult(TParameters...)> {};

        ///
        /// <summary>
        ///   Extracts the invocable's result type and signature for a functor or lambda by forwarding to its operator().
        /// </summary>
        ///
        template<typename TClass>
        requires requires { &std::remove_reference_t<TClass>::operator(); }
        struct invocable_traits<TClass> : invocable_traits<decltype(&std::remove_reference_t<TClass>::operator())> {};

        template<typename TInvocable>
        using invocable_result_type = typename invocable_traits<std::decay_t<TInvocable>>::result_type;

        template<typename TInvocable>
        using invocable_parameters_type = typename invocable_traits<std::decay_t<TInvocable>>::parameters_type;

        template<typename TInvocable>
        using invocable_signature_type = typename invocable_traits<std::decay_t<TInvocable>>::signature_type;

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

        template<typename... Invocables, typename... TParameters>
        consteval std::size_t find_invocable_index(std::tuple<TParameters...>* = nullptr)
        {
            constexpr std::size_t match_count = ((std::is_invocable_v<Invocables, TParameters...> ? 1 : 0) + ...);

            static_assert(match_count > 0, "No matching function found for the provided arguments.");
            static_assert(match_count <= 1, "Ambiguous call: multiple matching functions found for the provided arguments.");

            constexpr bool matches[] = { std::is_invocable_v<Invocables, TParameters...>... };
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

        template<typename... TParameters, typename... Invocables>
        struct find_invocable<std::tuple<TParameters...>, Invocables...>
        {
            using type = std::tuple_element_t<
                find_invocable_index<Invocables...>(static_cast<std::tuple<TParameters...>*>(nullptr)),
                std::tuple<Invocables...>>;
        };
    }

    ///
    /// <summary>
    ///   Deduce result type of an invocable.
    /// </summary>
    ///
    template<typename TInvocable>
    using invocable_result_type = detail::invocable_result_type<TInvocable>;

    ///
    /// <summary>
    ///   Deduce an invocable's signature which is TResult(TParameters...)
    /// </summary>
    template<typename TInvocable>
    using invocable_parameters_type = detail::invocable_parameters_type<TInvocable>;

    ///
    /// <summary>
    ///   Deduce an invocable's signature which is TResult(TParameters...)
    /// </summary>
    template<typename TInvocable>
    using invocable_signature_type = detail::invocable_signature_type<TInvocable>;

    ///
    /// <summary>
    ///     Concept of an invocable that can be called with its deduced parameters.
    /// </summary>
    ///
    template<typename TInvocable>
    concept Invocable =
        []<typename... TParameters>(std::type_identity<std::tuple<TParameters...>>)
        {
            return std::invocable<TInvocable, TParameters...>;
        }(std::type_identity<invocable_parameters_type<TInvocable>>{});

    ///
    /// <summary>
    ///   Deduce the invocable based on its parameters from the series of invocables.
    /// </summary>
    ///
    template<typename Parameters, typename... Invocables>
    using find_invocable_type = typename detail::find_invocable<Parameters, Invocables...>::type;

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
        []<typename TInvocable, typename... RemaingInvocables>(std::type_identity<TInvocable>, std::type_identity<RemaingInvocables>...)
        {
            return (std::is_same_v<invocable_result_type<TInvocable>, invocable_result_type<RemaingInvocables>> && ...);
        }(std::type_identity<Invocables>{}...);

    ///
    /// <summary>
    ///   Verify that each invocable in the series has a unique signature -> TResult(TParameters...).
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

    template<typename... TParameters, typename... Invocables>
    struct are_invocables_parameters_unique<std::tuple<TParameters...>, Invocables...>
         : std::bool_constant<((std::is_invocable_v<Invocables, TParameters...> ? 1 : 0) + ...) == 1> {};

    template<typename Parameters, typename... Invocables>
    concept AreInvocablesParametersUnique = are_invocables_parameters_unique<Parameters, Invocables...>::value;

    ///
    /// <summary>
    ///     Concept of an invocable that supports nullability.
    /// </summary>
    ///
    template<typename TInvocable>
    concept InvocableNullable =
        Invocable<TInvocable> &&
        requires(const std::remove_reference_t<TInvocable>& invocable)
        {
            static_cast<bool>(invocable);
        };

    ///
    /// <summary>
    ///     Concept of invocable with a void result.
    /// </summary>
    ///
    template<typename TInvocable>
    concept InvocableVoidResult =
        Invocable<TInvocable> &&
        std::is_void_v<invocable_result_type<TInvocable>>;

    ///
    /// <summary>
    ///     Concept of invocable with a result (non-void).
    /// </summary>
    ///
    template<typename TInvocable>
    concept InvocableWithResult =
        Invocable<TInvocable> &&
        !InvocableVoidResult<TInvocable>;

    ///
    /// <summary>
    ///     Concept of an invocable that is both nullable with a result (non-void).
    /// </summary>
    ///
    template<typename TInvocable>
    concept InvocableNullableWithResult =
        InvocableNullable<TInvocable> &&
        InvocableWithResult<TInvocable>;
}
