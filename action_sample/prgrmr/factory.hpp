#pragma once

#include "./delegates.hpp"
#include "./concepts/arguments.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>
#include <memory>
#include <utility>

namespace prgrmr
{
///
/// <summary>
///   A factory class composed of delegates.
///
///   Constructs instances of TClass using one of several configured delegate
///   signatures without a key/map mechanism.
/// </summary>
///
/// <seealso cref="delegates"/>
///
template<typename TClass, concepts::invocable::Invocable... TInvocables>
requires concepts::arguments::IsNotEmpty<TInvocables...> &&
         concepts::invocable::IsSameResultType<TInvocables...> &&
         ((std::same_as<concepts::invocable::invocable_result_type<TInvocables>, TClass*> ||
           std::same_as<concepts::invocable::invocable_result_type<TInvocables>, std::unique_ptr<TClass>> ||
           std::same_as<concepts::invocable::invocable_result_type<TInvocables>, std::shared_ptr<TClass>> ||
           std::convertible_to<concepts::invocable::invocable_result_type<TInvocables>, TClass*>) && ...)
class factory final
{
public:
   using delegates_type = delegates<TInvocables...>;
   using result_type = concepts::invocable::invocables_result_type<TInvocables...>;

   factory() = default;
   factory(const factory&) = delete;
   factory(factory&&) = default;

   ~factory() = default;

   factory& operator=(const factory&) = delete;
   factory& operator=(factory&&) = default;

   /// <summary>
   ///   Constructs a factory with delegates.
   /// </summary>
   explicit factory(delegates_type&& delegates)
   : _delegates{ std::move(delegates) }
   {
   }

   /// <summary>
   ///   Constructs a factory with an invocable for each delegate.
   /// </summary>
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables) &&
             !std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, delegates_type>)
   explicit factory(TOtherInvocables&&... invocables)
   : _delegates{ std::forward<TOtherInvocables>(invocables)... }
   {
   }

   /// <summary>
   ///   Constructs a factory from a tuple of invocables.
   /// </summary>
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   explicit factory(std::tuple<TOtherInvocables...>&& invocables_tuple)
   : _delegates{ std::move(invocables_tuple) }
   {
   }

   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   explicit factory(const std::tuple<TOtherInvocables...>& invocables_tuple)
   : _delegates{ invocables_tuple }
   {
   }

   /// <summary>
   ///   Resets all delegates to another collection of delegates.
   /// </summary>
   void reset(delegates_type&& delegates)
   {
      _delegates.reset(std::move(delegates));
   }

   /// <summary>
   ///   Resets delegates that match each of the provided invocables.
   /// </summary>
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires concepts::arguments::IsNotEmpty<TOtherInvocables...> &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, delegates_type>)
   void reset(TOtherInvocables&&... invocables)
   {
      _delegates.reset(std::forward<TOtherInvocables>(invocables)...);
   }

   /// <summary>
   ///   Resets the delegate that matches the provided invocable.
   /// </summary>
   template<concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t> &&
             !std::same_as<std::remove_cvref_t<TOtherInvocable>, delegates_type>)
   void reset(TOtherInvocable&& invocable)
   {
      _delegates.reset(std::forward<TOtherInvocable>(invocable));
   }

   /// <summary>
   ///   Resets all delegates with a tuple of invocables.
   /// </summary>
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(std::tuple<TOtherInvocables...>&& invocables_tuple)
   {
      _delegates.reset(std::move(invocables_tuple));
   }

   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(const std::tuple<TOtherInvocables...>& invocables_tuple)
   {
      _delegates.reset(invocables_tuple);
   }

   /// <summary>
   ///   Resets a specific delegate by its signature or invocable type.
   /// </summary>
   template<concepts::invocable::Invocable TInvocable, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(TOtherInvocable&& invocable)
   {
      _delegates.template reset<TInvocable>(std::forward<TOtherInvocable>(invocable));
   }

   /// <summary>
   ///   Resets a specific delegate by its index position.
   /// </summary>
   template<size_t TIndex, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(TOtherInvocable&& invocable)
   {
      _delegates.template reset<TIndex>(std::forward<TOtherInvocable>(invocable));
   }

   /// <summary>
   ///   Resets all delegates to their empty state.
   /// </summary>
   void reset(std::nullptr_t = nullptr)
   {
      _delegates.reset();
   }

   /// <summary>
   ///   Resets a specific delegate by its signature or invocable type to its empty state.
   /// </summary>
   template<concepts::invocable::Invocable TInvocable>
   void reset(std::nullptr_t = nullptr)
   {
      _delegates.template reset<TInvocable>(nullptr);
   }

   /// <summary>
   ///   Resets a specific delegate by its index position to its empty state.
   /// </summary>
   template<size_t TIndex>
   void reset(std::nullptr_t = nullptr)
   {
      _delegates.template reset<TIndex>(nullptr);
   }

   /// <summary>
   ///   Constructs an instance using the specified delegate signature.
   /// </summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto make(TParameters&&... parameters) const
   {
      return _delegates.template
               invoke_optional<TInvocable>(std::forward<TParameters>(parameters)...)
               .value_or(result_type{});
   }

   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   template<typename... TParameters>
   auto make(TParameters&&... parameters) const
   {
      return _delegates
             .invoke_optional(std::forward<TParameters>(parameters)...)
             .value_or(result_type{});
   }

   /// <summary>
   ///   Constructs an instance using the delegate at the specified index.
   /// </summary>
   template<size_t TIndex, typename... TParameters>
   auto make(TParameters&&... parameters) const
   {
      return _delegates.template
               invoke_optional<TIndex>(std::forward<TParameters>(parameters)...)
               .value_or(result_type{});
   }

   /// <summary>Constructs an instance with unique ownership.</summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto make_unique(TParameters&&... parameters) const
   {
      return to_unique(make<TInvocable>(std::forward<TParameters>(parameters)...));
   }

   template<typename... TParameters>
   auto make_unique(TParameters&&... parameters) const
   {
      return to_unique(make(std::forward<TParameters>(parameters)...));
   }

   template<size_t TIndex, typename... TParameters>
   auto make_unique(TParameters&&... parameters) const
   {
      return to_unique(make<TIndex>(std::forward<TParameters>(parameters)...));
   }

   /// <summary>Constructs an instance with shared ownership.</summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto make_shared(TParameters&&... parameters) const
   {
      return to_shared(make<TInvocable>(std::forward<TParameters>(parameters)...));
   }

   template<typename... TParameters>
   auto make_shared(TParameters&&... parameters) const
   {
      return to_shared(make(std::forward<TParameters>(parameters)...));
   }

   template<size_t TIndex, typename... TParameters>
   auto make_shared(TParameters&&... parameters) const
   {
      return to_shared(make<TIndex>(std::forward<TParameters>(parameters)...));
   }

   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   void swap(factory& other)
   {
      _delegates.swap(other._delegates);
   }

private:
   static auto to_unique(auto instance)
   {
      using instance_type = decltype(instance);

      if constexpr (std::same_as<instance_type, std::unique_ptr<TClass>>)
      {
         return instance;
      }
      else
      {
         return std::unique_ptr<TClass>{ instance };
      }
   }

   static auto to_shared(auto instance)
   {
      using instance_type = decltype(instance);

      if constexpr (std::same_as<instance_type, std::shared_ptr<TClass>>)
      {
         return instance;
      }
      else if constexpr (std::same_as<instance_type, std::unique_ptr<TClass>>)
      {
         return std::shared_ptr<TClass>{ std::move(instance) };
      }
      else
      {
         return (instance != nullptr)
              ? std::shared_ptr<TClass>{ instance }
              : nullptr;
      }
   }

   delegates_type _delegates;
};
}
