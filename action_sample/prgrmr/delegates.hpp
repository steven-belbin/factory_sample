#pragma once

#include "./delegate.hpp"
#include "./concepts/arguments.hpp"
#include "./concepts/invocable.hpp"
#include <concepts>
#include <cstddef>
#include <tuple>
#include <utility>

namespace prgrmr
{
///
/// <summary>
///   Contains one to many delegates that can be configured at runtime.
/// </summary>
///
template<concepts::invocable::Invocable... TInvocables>
requires concepts::arguments::IsNotEmpty<TInvocables...>
class delegates final
{
public:
   using delegates_type = std::tuple<delegate<TInvocables>...>;

   delegates() = default;
   delegates(const delegates&) = delete;
   delegates(delegates&&) = default;

   ~delegates() = default;

   delegates& operator=(const delegates&) = delete;
   delegates& operator=(delegates&&) = default;

   ///
   /// <summary>
   ///   Constructs an instance with delegates.
   /// </summary>
   ///
   /// <param name="delegates">A container of delegates.</param>
   ///
   explicit delegates(delegates_type&& delegates)
   : _delegates{ std::move(delegates) }
   {
   }

   ///
   /// <summary>
   ///   Constructs an instance with an invocable for each delegate.
   /// </summary>
   ///
   /// <param name="invocables">The invocables to initialize the delegates with.</param>
   ///
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables) &&
             !std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, delegates_type>)
   explicit delegates(TOtherInvocables&&... invocables)
   {
      (reset(std::forward<TOtherInvocables>(invocables)), ...);
   }

   ///
   /// <summary>
   ///   Constructs an instance from a tuple of invocables.
   /// </summary>
   ///
   /// <param name="invocables_tuple">A tuple containing an invocable for each delegate.</param>
   ///
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   explicit delegates(std::tuple<TOtherInvocables...>&& invocables_tuple)
   {
      std::apply([this](auto&&... invocables)
      {
         this->reset(std::forward<decltype(invocables)>(invocables)...);
      }, std::move(invocables_tuple));
   }

   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   explicit delegates(const std::tuple<TOtherInvocables...>& invocables_tuple)
   {
      std::apply([this](const auto&... invocables)
      {
         this->reset(invocables...);
      }, invocables_tuple);
   }

   ///
   /// <summary>
   ///   Resets all delegates to another collection of delegates.
   /// </summary>
   ///
   /// <param name="delegates">A container of delegates.</param>
   ///
   void reset(delegates_type&& delegates)
   {
      _delegates = std::move(delegates);
   }

   ///
   /// <summary>
   ///   Resets delegates that match each of the provided invocables.
   /// </summary>
   ///
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (concepts::arguments::IsAtLeast<2, TOtherInvocables...> &&
             !std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, delegates_type>)
   void reset(TOtherInvocables&&... invocables)
   {
      (reset(std::forward<TOtherInvocables>(invocables)), ...);
   }

   ///
   /// <summary>
   ///   Resets the delegate that matches the provided invocable.
   /// </summary>
   ///
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t> &&
             !std::same_as<std::remove_cvref_t<TOtherInvocable>, delegates_type>)
   void reset(TOtherInvocable&& invocable)
   {
      constexpr std::size_t index = concepts::invocable::detail::find_matching_delegate_index<TOtherInvocable, TInvocables...>();

      get<index>().reset(std::forward<TOtherInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Resets all delegates with a tuple of invocables.
   /// </summary>
   ///
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(std::tuple<TOtherInvocables...>&& invocables_tuple)
   {
      std::apply([this](auto&&... invocables)
      {
         this->reset(std::forward<decltype(invocables)>(invocables)...);
      }, std::move(invocables_tuple));
   }

   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(const std::tuple<TOtherInvocables...>& invocables_tuple)
   {
      std::apply([this](const auto&... invocables)
      {
         this->reset(invocables...);
      }, invocables_tuple);
   }

   ///
   /// <summary>
   ///   Resets all delegates to their empty state.
   /// </summary>
   ///
   void reset(std::nullptr_t = nullptr)
   {
      [this]<std::size_t... Is>(std::index_sequence<Is...>)
      {
         (std::get<Is>(_delegates).reset(), ...);
      }(std::index_sequence_for<TInvocables...>{});
   }

   ///
   /// <summary>
   ///   Get a specific delegate by its signature or invocable type.
   /// </summary>
   ///
   /// <returns>A const reference to the registered delegate.</returns>
   ///
   template<concepts::invocable::Invocable TInvocable>
   const auto& get() const
   {
      constexpr std::size_t index = concepts::invocable::detail::find_delegate_index<TInvocable, TInvocables...>();

      return std::get<index>(_delegates);
   }

   ///
   /// <summary>
   ///   Get a specific delegate by its signature or invocable type.
   /// </summary>
   ///
   /// <returns>A reference to the registered delegate.</returns>
   ///
   template<concepts::invocable::Invocable TInvocable>
   auto& get()
   {
      constexpr std::size_t index = concepts::invocable::detail::find_delegate_index<TInvocable, TInvocables...>();

      return std::get<index>(_delegates);
   }

   ///
   /// <summary>
   ///   Get a specific delegate by its index position.
   /// </summary>
   ///
   /// <returns>A const reference to the registered delegate.</returns>
   ///
   template<std::size_t TIndex>
   const auto& get() const
   {
      return std::get<TIndex>(_delegates);
   }

   ///
   /// <summary>
   ///   Get a specific delegate by its index position.
   /// </summary>
   ///
   /// <returns>A reference to the registered delegate.</returns>
   ///
   template<std::size_t TIndex>
   auto& get()
   {
      return std::get<TIndex>(_delegates);
   }

   ///
   /// <summary>
   ///   Resets a specific delegate by its signature or invocable type to its empty state.
   /// </summary>
   ///
   template<concepts::invocable::Invocable TInvocable>
   void reset(std::nullptr_t = nullptr)
   {
      get<TInvocable>().reset();
   }

   ///
   /// <summary>
   ///   Resets a specific delegate by its signature or invocable type to another invocable.
   /// </summary>
   ///
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<concepts::invocable::Invocable TInvocable, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(TOtherInvocable&& invocable)
   {
      get<TInvocable>().reset(std::forward<TOtherInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Resets a specific delegate by its index position to its empty state.
   /// </summary>
   ///
   template<std::size_t TIndex>
   void reset(std::nullptr_t = nullptr)
   {
      get<TIndex>().reset();
   }

   ///
   /// <summary>
   ///   Resets a specific delegate by its index position to another invocable.
   /// </summary>
   ///
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<std::size_t TIndex, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(TOtherInvocable&& invocable)
   {
      get<TIndex>().reset(std::forward<TOtherInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Invoke a specific delegate by its signature or invocable type.
   /// </summary>
   ///
   /// <param name="parameters">The arguments to pass to the delegate.</param>
   ///
   /// <exception cref="std::bad_function_call">When the delegate has not been registered.</exception>
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   requires concepts::invocable::CanInvoke<TInvocable, TParameters...>
   auto invoke(TParameters&&... parameters) const
   {
      return get<TInvocable>().invoke(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   ///
   template<typename... TParameters>
   requires concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>
   auto invoke(TParameters&&... parameters) const
   {
      using TInvocable = concepts::invocable::find_invocable_type<std::tuple<TParameters...>, TInvocables...>;

      return invoke<TInvocable>(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   ///
   template<typename... TParameters>
   requires concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>
   auto operator()(TParameters&&... parameters) const
   {
      return invoke(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invoke a specific delegate by its tuple index.
   /// </summary>
   ///
   /// <param name="parameters">The arguments to pass to the delegate.</param>
   ///
   /// <exception cref="std::bad_function_call">When the delegate has not been registered.</exception>
   ///
   template<std::size_t TIndex, typename... TParameters>
   requires concepts::invocable::CanInvoke<std::tuple_element_t<TIndex, std::tuple<TInvocables...>>, TParameters...>
   auto invoke(TParameters&&... parameters) const
   {
      return get<TIndex>().invoke(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Locates the delegate by its signature and when it's registered, invokes it.
   /// </summary>
   ///
   /// <param name="parameters">The arguments to pass to the delegate.</param>
   ///
   /// <returns>
   ///   When the delegate is registered, the result is wrapped in std::optional.
   ///   Otherwise std::nullopt indicates that the delegate isn't registered.
   /// </returns>
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   requires concepts::invocable::InvocableWithResult<TInvocable> &&
            concepts::invocable::CanInvoke<TInvocable, TParameters...>
   auto invoke_optional(TParameters&&... parameters) const
   {
      return get<TInvocable>().invoke_optional(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Locates the delegate by its signature and when it's registered, invokes it.
   /// </summary>
   ///
   /// <param name="parameters">The arguments to pass to the delegate.</param>
   ///
   /// <returns>
   ///   When the delegate is registered, the result is wrapped in std::optional.
   ///   Otherwise std::nullopt indicates that the delegate isn't registered.
   /// </returns>
   ///
   template<typename... TParameters>
   requires concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>
   auto invoke_optional(TParameters&&... parameters) const
   {
      using TInvocable = concepts::invocable::find_invocable_type<std::tuple<TParameters...>, TInvocables...>;

      return invoke_optional<TInvocable>(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Locates the delegate by its tuple index and if one is registered, then invokes it.
   /// </summary>
   ///
   /// <param name="parameters">The arguments to pass to the delegate.</param>
   ///
   /// <returns>
   ///  An optional containing the result of invoking the delegate.
   ///  Otherwise, std::nullopt when the delegate is not defined.
   /// </returns>
   ///
   template<std::size_t TIndex, typename... TParameters>
   requires concepts::invocable::InvocableWithResult<std::tuple_element_t<TIndex, std::tuple<TInvocables...>>> &&
            concepts::invocable::CanInvoke<std::tuple_element_t<TIndex, std::tuple<TInvocables...>>, TParameters...>
   auto invoke_optional(TParameters&&... parameters) const
   {
      return get<TIndex>().invoke_optional(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(delegates& other)
   {
      _delegates.swap(other._delegates);
   }

private:
   delegates_type _delegates;
};
}
