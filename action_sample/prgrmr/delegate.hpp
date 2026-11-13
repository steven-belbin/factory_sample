#pragma once

#include "./functional.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>
#include <functional>

namespace prgrmr
{
///
/// <summary>
///   Contains an invocable that references an invocable or not at runtime.
/// </summary>
///
template<typename TInvocable,
         typename TParametersTuple = concepts::invocable::invocable_parameters_type<TInvocable>>
class delegate;

template<typename TInvocable, typename... TParameters>
class delegate<TInvocable, std::tuple<TParameters...>> final
{
public:
   using signature_type = concepts::invocable::invocable_signature_type<TInvocable>;
   using invocable_type = std::function<signature_type>;
   using parameters_type = concepts::invocable::invocable_parameters_type<TInvocable>;
   using result_type = concepts::invocable::invocable_result_type<TInvocable>;

   delegate() = default;
   delegate(const delegate&) = delete;
   delegate(delegate&&) = default;

   ~delegate() = default;

   delegate& operator=(const delegate&) = delete;
   delegate& operator=(delegate&&) = default;

   ///
   /// <summary>
   ///   Construct and registers to an invocable.
   /// </summary>
   ///
   /// <param name="invocable">An invocable.</param>
   ///
   template<typename TOtherInvocable>
   requires (!std::is_same_v<std::remove_cvref_t<TOtherInvocable>, delegate> && std::constructible_from<invocable_type, TOtherInvocable>)
   explicit delegate(TOtherInvocable&& invocable)
   : _invocable{ std::forward<TOtherInvocable>(invocable) }
   {
   }

   ///
   /// <summary>
   ///   Get the invocable.
   /// </summary>
   ///
   /// <returns>A reference to the registered invocable.</returns>
   ///
   /// <remarks>When no invocable has been registered, then reference will be an empty invocable <--> nullptr.</remarks>
   ///
   const auto& invocable() const
   {
      return _invocable;
   }

   ///
   /// <summary>
   ///   Resets the registered invocable to nullptr.
   /// </summary>
   ///
   void reset(std::nullptr_t = nullptr)
   {
      _invocable = nullptr;
   }

   ///
   /// <summary>
   ///   Registers or replaces the invocable.
   /// </summary>
   ///
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<typename TOtherInvocable>
   requires std::constructible_from<invocable_type, TOtherInvocable>
   void reset(TOtherInvocable&& invocable)
   {
      _invocable = std::forward<TOtherInvocable>(invocable);
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
      return _invocable(std::forward<TParameters>(parameters)...);
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
      return prgrmr::invoke_optional(_invocable, std::forward<TParameters>(parameters)...);
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
      _invocable.swap(other._invocable);
   }

   ///
   /// <summary>
   ///   Assigns the invocable from another invocable.
   /// </summary>
   ///
   /// <param name="other">The invocable to assign from.</param>
   ///
   template<typename TOtherInvocable>
   requires std::constructible_from<invocable_type, TOtherInvocable>
   auto& operator=(TOtherInvocable&& invocable) noexcept
   {
      reset(std::forward<TOtherInvocable>(invocable));
      return *this;
   }

   ///
   /// <summary>
   ///   Unregisters the invocable.
   /// </summary>
   ///
   auto& operator=(const std::nullptr_t)
   {
      reset();
      return *this;
   }

   ///
   /// <summary>
   ///  Checks whether an invocable is registered.
   /// </summary>
   ///
   operator bool() const noexcept
   {
      return static_cast<bool>(_invocable);
   }

   ///
   /// <summary>
   ///  Checks whether an invocable is not registered.
   /// </summary>
   ///
   auto operator!() const noexcept
   {
      return !this->operator bool();
   }

private:
   invocable_type _invocable;
};
}
