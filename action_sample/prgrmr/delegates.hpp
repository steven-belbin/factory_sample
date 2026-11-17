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
///   Contains one to many delegates which can be registered and unregistered at runtime.
/// </summary>
///
template<concepts::invocable::Invocable... TInvocables>
class delegates final
{
public:
   static_assert(concepts::arguments::IsNotEmpty<TInvocables...>, "The argument list cannot be empty.");

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
   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables)) &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TArgs...>>>, delegates_type>)
   explicit delegates(TArgs&&... invocables)
   : _delegates{ delegate<TInvocables>{ std::forward<TArgs>(invocables) }... }
   {
   }

   ///
   /// <summary>
   ///   Constructs an instance from a tuple of invocables.
   /// </summary>
   ///
   /// <param name="invocables_tuple">A tuple containing an invocable for each delegate.</param>
   ///
   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   explicit delegates(std::tuple<TArgs...>&& invocables_tuple)
   : _delegates{ [this]<std::size_t... Is>(std::tuple<TArgs...>&& t, std::index_sequence<Is...>) {
        return std::make_tuple(delegate<TInvocables>{ std::get<Is>(std::move(t)) }...);
     }(std::move(invocables_tuple), std::index_sequence_for<TInvocables...>{}) }
   {
   }

   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   explicit delegates(const std::tuple<TArgs...>& invocables_tuple)
   : _delegates{ [this]<std::size_t... Is>(const std::tuple<TArgs...>& t, std::index_sequence<Is...>) {
        return std::make_tuple(delegate<TInvocables>{ std::get<Is>(t) }...);
     }(invocables_tuple, std::index_sequence_for<TInvocables...>{}) }
   {
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
   ///   Resets all delegates with the provided invocables.
   /// </summary>
   ///
   template<typename... TArgs>
   requires (sizeof...(TInvocables) > 1 && sizeof...(TArgs) == sizeof...(TInvocables)) &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TArgs...>>>, delegates_type>)
   void reset(TArgs&&... invocables)
   {
      _delegates = std::make_tuple(delegate<TInvocables>{ std::forward<TArgs>(invocables) }...);
   }

   ///
   /// <summary>
   ///   Resets the delegate that matches the provided invocable.
   /// </summary>
   ///
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t> &&
             !std::same_as<std::remove_cvref_t<TTargetInvocable>, delegates_type>)
   void reset(TTargetInvocable&& invocable)
   {
      constexpr std::size_t index = concepts::invocable::detail::find_matching_delegate_index<TTargetInvocable, TInvocables...>();
      get<index>().reset(std::forward<TTargetInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Resets all delegates with a tuple of invocables.
   /// </summary>
   ///
   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   void reset(std::tuple<TArgs...>&& invocables_tuple)
   {
      _delegates = [this]<std::size_t... Is>(std::tuple<TArgs...>&& t, std::index_sequence<Is...>) {
         return std::make_tuple(delegate<TInvocables>{ std::get<Is>(std::move(t)) }...);
      }(std::move(invocables_tuple), std::index_sequence_for<TInvocables...>{});
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
      static_assert(index < sizeof...(TInvocables), "No matching delegate found for the requested signature.");

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
      static_assert(index < sizeof...(TInvocables), "No matching delegate found for the requested signature.");

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
   template<concepts::invocable::Invocable TInvocable, typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t>)
   void reset(TTargetInvocable&& invocable)
   {
      get<TInvocable>().reset(std::forward<TTargetInvocable>(invocable));
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
   template<std::size_t TIndex, typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t>)
   void reset(TTargetInvocable&& invocable)
   {
      get<TIndex>().reset(std::forward<TTargetInvocable>(invocable));
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
   auto invoke(TParameters&&... parameters) const
   {
      static_assert(concepts::invocable::CanInvoke<TInvocable, TParameters...>,
                    "The provided parameters do not match the requested delegate signature.");

      if constexpr (concepts::invocable::CanInvoke<TInvocable, TParameters...>)
      {
         return get<TInvocable>().invoke(std::forward<TParameters>(parameters)...);
      }
   }

   ///
   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   ///
   template<typename... TParameters>
   auto invoke(TParameters&&... parameters) const
   {
      static_assert(concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>,
                    "No matching delegate found in delegates for the provided parameters.");

      if constexpr (concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>)
      {
         using TInvocable = concepts::invocable::find_invocable_type<std::tuple<TParameters...>, TInvocables...>;

         return invoke<TInvocable>(std::forward<TParameters>(parameters)...);
      }
   }

   ///
   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   ///
   template<typename... TParameters>
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
   auto invoke(TParameters&&... parameters) const
   {
      using target_invocable = std::tuple_element_t<TIndex, std::tuple<TInvocables...>>;
      static_assert(concepts::invocable::CanInvoke<target_invocable, TParameters...>,
                    "The provided parameters do not match the delegate signature at the specified index.");

      if constexpr (concepts::invocable::CanInvoke<target_invocable, TParameters...>)
      {
         return get<TIndex>().invoke(std::forward<TParameters>(parameters)...);
      }
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
   requires concepts::invocable::InvocableWithResult<TInvocable>
   auto invoke_optional(TParameters&&... parameters) const
   {
      static_assert(concepts::invocable::CanInvoke<TInvocable, TParameters...>,
                    "The provided parameters do not match the requested delegate signature.");

      if constexpr (concepts::invocable::CanInvoke<TInvocable, TParameters...>)
      {
         return get<TInvocable>().invoke_optional(std::forward<TParameters>(parameters)...);
      }
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
   auto invoke_optional(TParameters&&... parameters) const
   {
      static_assert(concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>,
                    "No matching delegate found in delegates for the provided parameters.");

      if constexpr (concepts::invocable::CanInvokeAny<std::tuple<TParameters...>, TInvocables...>)
      {
         using TInvocable = concepts::invocable::find_invocable_type<std::tuple<TParameters...>, TInvocables...>;

         return invoke_optional<TInvocable>(std::forward<TParameters>(parameters)...);
      }
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
   requires concepts::invocable::InvocableWithResult<std::tuple_element_t<TIndex, std::tuple<TInvocables...>>>
   auto invoke_optional(TParameters&&... parameters) const
   {
      using target_invocable = std::tuple_element_t<TIndex, std::tuple<TInvocables...>>;
      static_assert(concepts::invocable::CanInvoke<target_invocable, TParameters...>,
                    "The provided parameters do not match the delegate signature at the specified index.");

      if constexpr (concepts::invocable::CanInvoke<target_invocable, TParameters...>)
      {
         return get<TIndex>().invoke_optional(std::forward<TParameters>(parameters)...);
      }
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
