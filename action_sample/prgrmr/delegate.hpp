#pragma once

#include "./functional.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>
#include <functional>

namespace prgrmr
{
///
/// <summary>
///   A delegate can be assigned to an invocable or be in an empty state.
///   Its state can change during runtime.
///
///   It's capable of an optional invocation which returns its result wrapped in std::optional.
///   When it's in an empty state, then `std::nullopt` is returned.
/// </summary>
///
/// <remarks>
///   To distingush from a regular function pointer or std::function, a delegate explicitly
///   supports the delegate to be an empty state forcing clients to verify for the presence
///   of a result type after invocation of the delegates member function.
///
///   A pitfall of always returning an `std::optional<result_type>` the client code is
///   responsibillty to explicitly handle invocations when the delegate is its empty state.
/// </remarks>
///
template<typename TInvocable,
         typename TParametersTuple = concepts::invocable::invocable_parameters_type<TInvocable>>
class delegate;

template<typename TInvocable, typename... TParameters>
class delegate<TInvocable, std::tuple<TParameters...>> final
{
public:
   using traits = concepts::invocable::invocable_traits<TInvocable>;
   using signature_type = traits::signature_type;
   using parameters_type = traits::parameters_type;
   using result_type = traits::result_type;

private:
   using function_type = std::function<signature_type>;
   function_type _function;

public:
   delegate() = default;
   delegate(const delegate&) = delete;
   delegate(delegate&&) = default;

   ~delegate() = default;

   delegate& operator=(const delegate&) = delete;
   delegate& operator=(delegate&&) = default;

   ///
   /// <summary>
   ///   Construct and assigns an invocable.
   /// </summary>
   ///
   /// <param name="invocable">An invocable.</param>
   ///
   template<typename TOtherInvocable>
   requires (!std::is_same_v<std::remove_cvref_t<TOtherInvocable>, delegate> && std::constructible_from<function_type, TOtherInvocable>)
   explicit delegate(TOtherInvocable&& invocable)
   : _function{ std::forward<TOtherInvocable>(invocable) }
   {
   }

   ///
   /// <summary>
   ///   Resets the invocable to its empty state.
   /// </summary>
   ///
   void reset(std::nullptr_t = nullptr)
   {
      _function = nullptr;
   }

   ///
   /// <summary>
   ///   Resets the invocable to another invocable.
   /// </summary>
   ///
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<typename TOtherInvocable>
   requires std::constructible_from<function_type, TOtherInvocable>
   void reset(TOtherInvocable&& invocable)
   {
      _function = std::forward<TOtherInvocable>(invocable);
   }

   ///
   /// <summary>
   ///   Invokes the registered invocable with the provided parameters.
   /// </summary>
   ///
   /// <param name="parameters">The parameters to pass to the invocable.</param>
   ///
   auto invoke(TParameters... parameters) const
   {
      return _function(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invokes the registered invocable with the provided parameters.
   /// </summary>
   ///
   /// <param name="parameters">The parameters to pass to the invocable.</param>
   ///
   auto operator()(TParameters... parameters) const
   {
      return invoke(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invokes the registered invocable with the provided parameters, if it is registered.
   /// </summary>
   ///
   /// <param name="parameters">The parameters to pass to the invocable.</param>
   ///
   /// <returns>
   ///   When the invocable is registered, the result is wrapped in std::optional.
   ///   Otherwise std::nullopt indicates that the invocable isn't registered.
   /// </returns>
   ///
   /// <seealso cref="invoke_optional<function_t>"/>
   ///
   auto invoke_optional(TParameters... parameters) const
   {
      return prgrmr::invoke_optional(_function, std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(delegate& other)
   {
      _function.swap(other._function);
   }

   ///
   /// <summary>
   ///   Assign the delegate to its empty state.
   /// </summary>
   ///
   auto& operator=(const std::nullptr_t)
   {
      reset();
      return *this;
   }

   ///
   /// <summary>
   ///   Assign an invocable.
   /// </summary>
   ///
   /// <param name="other">The invocable.</param>
   ///
   template<typename TOtherInvocable>
   requires std::constructible_from<function_type, TOtherInvocable>
   auto& operator=(TOtherInvocable&& invocable) noexcept
   {
      reset(std::forward<TOtherInvocable>(invocable));
      return *this;
   }

   ///
   /// <summary>
   ///  Checks if the delegate has an invocable assigned.
   /// </summary>
   ///
   operator bool() const noexcept
   {
      return _function.operator bool();
   }
};
}
