#pragma once

#include "./delegates_map.hpp"
#include "./concepts/arguments.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>

namespace prgrmr
{
///
/// <summary>
///   The factory_map class allows to construct a class instance by a key.
///   <para>The class factory has 0..n entries in which each entry is identified by a unique key.</para>
///   <para>Any given entry can construct a class using any one of delegates, each with different number of arguments and different argument types.</para>
/// </summary>
///
/// <seealso cref="key_delegates_functions"/>
///
template<class TKey, concepts::invocable::Invocable... TInvocables>
class factory_map final
{
public:
   static_assert(concepts::arguments::IsNotEmpty<TInvocables...>, "The argument list cannot be empty.");

   static_assert(concepts::invocable::IsSameResultType<TInvocables...>,
                 "The argument list contains at least one delegate doesn't return the same type.");

   static_assert(concepts::invocable::AreInvocablesSignaturesUnique<TInvocables...>,
                 "The argument list contains at least one pair of delegates having the same signature.");

   using delegates_map_type = delegates_map<TKey, TInvocables...>;
   using delegates_type = delegates_map_type::delegates_type;
   using key_type = delegates_map_type::key_type;
   using result_type = concepts::invocable::invocables_result_type<TInvocables...>;

   factory_map() = default;
   factory_map(const factory_map&) = default;
   factory_map(factory_map&&) = default;

   ~factory_map() = default;

   /// <summary>
   ///   Constructs a factory_map.
   /// </summary>
   ///
   /// <param name="delegate">The delegates.</param>
   ///
   explicit factory_map(const delegates_map_type& delegates)
   : _delegates_map{ delegates }
   {
   }

   /// <summary>
   ///   Constructs a factory_map.
   /// </summary>
   ///
   /// <param name="delegate">The delegates.</param>
   ///
   explicit factory_map(delegates_map_type&& delegates)
   : _delegates_map{ std::forward<delegates_map_type>(delegates) }
   {
   }

   ///
   /// <summary>
   ///
   /// <summary>
   ///   Registers all the delegate under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   /// <param name="delegates">The delegates to be registered.</param>
   ///
   void reset(const key_type& key,
              delegates_type&& delegates)
   {
      _delegates_map.reset(key, std::move(delegates));
   }

   ///
   /// <summary>
   ///   Registers the delegates under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these delegate signatures.</param>
   /// <param name="invocables">The invocables that are to be registered.</param>
   ///
   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables)) &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TArgs...>>>, delegates_type>)
   void reset(const key_type& key,
              TArgs&&... invocables)
   {
      _delegates_map.reset(key, std::forward<TArgs>(invocables)...);
   }

   ///
   /// <summary>
   ///   Registers a tuple of invocables under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these delegate signatures.</param>
   /// <param name="invocables_tuple">The tuple of invocables to be registered.</param>
   ///
   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   void reset(const key_type& key,
              std::tuple<TArgs...>&& invocables_tuple)
   {
      _delegates_map.reset(key, std::move(invocables_tuple));
   }

   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   void reset(const key_type& key,
              const std::tuple<TArgs...>& invocables_tuple)
   {
      _delegates_map.reset(key, invocables_tuple);
   }

   ///
   /// <summary>
   ///   Registers a specific delegate by its index position.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key.</param>
   /// <param name="invocable">The invocable that is to be registered.</param>
   ///
   template<size_t TIndex, typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TTargetInvocable&& invocable)
   {
      _delegates_map.template reset<TIndex>(key, std::forward<TTargetInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Unregisters all the delegates that were registered with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the delegates were registered under.</param>
   ///
   void reset(const key_type& key,
              std::nullptr_t = nullptr)
   {
      _delegates_map.reset(key, nullptr);
   }

   ///
   /// <summary>
   ///   Erases the entry matching the given key from the map.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key.</param>
   ///
   void erase(const key_type& key)
   {
      _delegates_map.erase(key);
   }

   ///
   /// <summary>
   ///   Registers a single delegate under the given key by its signature.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this delegate.</param>
   /// <param name="invocable">The invocable that is to be registered.</param>
   ///
   template<concepts::invocable::Invocable TInvocable, typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TTargetInvocable&& invocable)
   {
      _delegates_map.template reset<TInvocable>(key, std::forward<TTargetInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Unregisters a specific delegate by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the delegate was registered under.</param>
   ///
   template<concepts::invocable::Invocable TInvocable>
   void reset(const key_type& key,
              std::nullptr_t = nullptr)
   {
      _delegates_map.template reset<TInvocable>(key, nullptr);
   }

   ///
   /// <summary>
   ///   Unregisters a specific delegate by its index position that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the delegate was registered under.</param>
   ///
   template<size_t TIndex>
   void reset(const key_type& key,
              std::nullptr_t = nullptr)
   {
      _delegates_map.template reset<TIndex>(key, nullptr);
   }

   ///
   /// <summary>
   ///   Constructs an instance of the class.
   /// </summary>
   ///
   /// <see cref="key_delegates_functions::invoke"/>
   ///
   /// <returns>An instance of the class.<returns>
   /// <returns>nullptr_t when the given key cannot be found.<returns>
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto construct(const key_type& key,
                  TParameters&&... parameters) const
   {
      return _delegates_map.template
             invoke_optional<TInvocable>(key, std::forward<TParameters>(parameters)...)
             .value_or(nullptr);
   }

   ///
   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   ///
   template<typename... TParameters>
   auto construct(const key_type& key,
                  TParameters&&... parameters) const
   {
      return _delegates_map.invoke_optional(key, std::forward<TParameters>(parameters)...)
                       .value_or(nullptr);
   }

   ///
   /// <summary>
   ///   Constructs an instance of the class.
   /// </summary>
   ///
   /// <see cref="key_delegates_functions::invoke"/>
   ///
   /// <returns>An instance of the class.<returns>
   /// <returns>nullptr_t when the given key cannot be found.<returns>
   ///
   template<size_t TIndex, typename... TParameters>
   auto construct(const key_type& key,
                  TParameters&&... parameters) const
   {
      return _delegates_map.template
             invoke_optional<TIndex>(key, std::forward<TParameters>(parameters)...)
             .value_or(nullptr);
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(factory_map& other)
   {
      _delegates_map.swap(other._delegates_map);
   }

private:
   delegates_map_type _delegates_map;
};
}
