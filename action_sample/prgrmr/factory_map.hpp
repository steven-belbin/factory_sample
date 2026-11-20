#pragma once

#include "./factory.hpp"
#include "./concepts/arguments.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <utility>

namespace prgrmr
{
///
/// <summary>
///   A collection of factories_map that are associated by key.
///
///   Allows applications at runtime to provide a key & configure
///   that key's factory so that application logic can create the
///   appropriate instance that matches key.
/// </summary>
///
/// <seealso cref="factory"/>
///
template<typename TClass, typename TKey, concepts::invocable::Invocable... TInvocables>
requires concepts::arguments::IsNotEmpty<TInvocables...> &&
         concepts::invocable::IsSameResultType<TInvocables...> &&
         ((std::same_as<concepts::invocable::invocable_result_type<TInvocables>, TClass*> ||
           std::same_as<concepts::invocable::invocable_result_type<TInvocables>, std::unique_ptr<TClass>> ||
           std::same_as<concepts::invocable::invocable_result_type<TInvocables>, std::shared_ptr<TClass>> ||
           std::convertible_to<concepts::invocable::invocable_result_type<TInvocables>, TClass*>) && ...)
class factory_map final
{
public:
   using key_type = TKey;
   using factory_type = factory<TClass, TInvocables...>;
   using factories_map_type = std::unordered_map<key_type, factory_type>;
   using delegates_type = typename factory_type::delegates_type;
   using result_type = concepts::invocable::invocables_result_type<TInvocables...>;

   factory_map() = default;
   factory_map(const factory_map&) = delete;
   factory_map(factory_map&&) = default;

   ~factory_map() = default;

   factory_map& operator=(const factory_map&) = delete;
   factory_map& operator=(factory_map&&) = default;

   /// <summary>
   ///   Constructs a factory_map from an existing map of factories_map.
   /// </summary>
   explicit factory_map(factories_map_type&& factories_map)
   : _factories_map{ std::move(factories_map) }
   {
   }

   ///
   /// <summary>
   ///   Clears all factories_map from the map.
   /// </summary>
   ///
   void clear()
   {
      _factories_map.clear();
   }

   ///
   /// <summary>
   ///   Checks if the map is empty.
   /// </summary>
   ///
   auto empty() const
   {
      return _factories_map.empty();
   }

   ///
   /// <summary>
   ///   Gets the number of entries in the map.
   /// </summary>
   ///
   auto size() const
   {
      return _factories_map.size();
   }

   ///
   /// <summary>
   ///   Get a pointer to the factory associated with the given key, or nullptr if not found.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching factory.</param>
   ///
   const factory_type* get(const key_type& key) const noexcept
   {
      const auto iter = _factories_map.find(key);

      return (iter != std::end(_factories_map))
           ? std::addressof(iter->second)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Get a pointer to the factory associated with the given key, or nullptr if not found.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching factory.</param>
   ///
   factory_type* get(const key_type& key) noexcept
   {
      const auto iter = _factories_map.find(key);

      return (iter != std::end(_factories_map))
           ? std::addressof(iter->second)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Get a reference to the factory associated with the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching factory.</param>
   /// <exception cref="std::out_of_range">When there is no factory registered with the given key.</exception>
   ///
   const factory_type& at(const key_type& key) const
   {
      return _factories_map.at(key);
   }

   ///
   /// <summary>
   ///   Get a reference to the factory associated with the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching factory.</param>
   /// <exception cref="std::out_of_range">When there is no factory registered with the given key.</exception>
   ///
   factory_type& at(const key_type& key)
   {
      return _factories_map.at(key);
   }

   ///
   /// <summary>
   ///   Registers a factory under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the factory.</param>
   /// <param name="factory">The factory to register.</param>
   ///
   void reset(const key_type& key,
              factory_type&& factory)
   {
      _factories_map.insert_or_assign(key, std::move(factory));
   }

   ///
   /// <summary>
   ///   Registers all the delegates under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   /// <param name="delegates">The delegates to be registered.</param>
   ///
   void reset(const key_type& key,
              delegates_type&& delegates)
   {
      _factories_map[key].reset(std::move(delegates));
   }

   ///
   /// <summary>
   ///   Registers the delegates under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these delegate signatures.</param>
   /// <param name="invocables">The invocables that are to be registered.</param>
   ///
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires concepts::arguments::IsNotEmpty<TOtherInvocables...> &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, delegates_type>) &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, factory_type>)
   void reset(const key_type& key,
              TOtherInvocables&&... invocables)
   {
      _factories_map[key].reset(std::forward<TOtherInvocables>(invocables)...);
   }

   ///
   /// <summary>
   ///   Resets the delegate matching the provided invocable under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key.</param>
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t> &&
             !std::same_as<std::remove_cvref_t<TOtherInvocable>, delegates_type> &&
             !std::same_as<std::remove_cvref_t<TOtherInvocable>, factory_type>)
   void reset(const key_type& key,
              TOtherInvocable&& invocable)
   {
      _factories_map[key].reset(std::forward<TOtherInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Registers a tuple of invocables under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these delegate signatures.</param>
   /// <param name="invocables_tuple">The tuple of invocables to be registered.</param>
   ///
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(const key_type& key,
              std::tuple<TOtherInvocables...>&& invocables_tuple)
   {
      _factories_map[key].reset(std::move(invocables_tuple));
   }

   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(const key_type& key,
              const std::tuple<TOtherInvocables...>& invocables_tuple)
   {
      _factories_map[key].reset(invocables_tuple);
   }

   ///
   /// <summary>
   ///   Registers a specific delegate by its index position.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key.</param>
   /// <param name="invocable">The invocable that is to be registered.</param>
   ///
   template<size_t TIndex, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TOtherInvocable&& invocable)
   {
      _factories_map[key].template reset<TIndex>(std::forward<TOtherInvocable>(invocable));
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
      if (auto* factory = get(key); factory != nullptr)
      {
         factory->reset();
      }
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
      _factories_map.erase(key);
   }

   ///
   /// <summary>
   ///   Registers a single delegate under the given key by its signature.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this delegate.</param>
   /// <param name="invocable">The invocable that is to be registered.</param>
   ///
   template<concepts::invocable::Invocable TInvocable, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TOtherInvocable&& invocable)
   {
      _factories_map[key].template reset<TInvocable>(std::forward<TOtherInvocable>(invocable));
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
      if (auto* factory = get(key); factory != nullptr)
      {
         factory->template reset<TInvocable>(nullptr);
      }
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
      if (auto* factory = get(key); factory != nullptr)
      {
         factory->template reset<TIndex>(nullptr);
      }
   }

   ///
   /// <summary>
   ///   Constructs an instance using the specified delegate signature.
   /// </summary>
   ///
   /// <returns>The constructed instance, or a default-constructed result if the key or delegate is missing.</returns>
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto make(const key_type& key,
             TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->template make<TInvocable>(std::forward<TParameters>(parameters)...)
           : result_type{};
   }

   ///
   /// <summary>
   ///   Invokes the delegate that matches the provided argument types.
   /// </summary>
   ///
   template<typename... TParameters>
   auto make(const key_type& key,
             TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->make(std::forward<TParameters>(parameters)...)
           : result_type{};
   }

   ///
   /// <summary>
   ///   Constructs an instance using the delegate at the specified index.
   /// </summary>
   ///
   /// <returns>The constructed instance, or a default-constructed result if the key or delegate is missing.</returns>
   ///
   template<size_t TIndex, typename... TParameters>
   auto make(const key_type& key,
             TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->template make<TIndex>(std::forward<TParameters>(parameters)...)
           : result_type{};
   }

   /// <summary>Constructs an instance with unique ownership.</summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto make_unique(const key_type& key, TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->template make_unique<TInvocable>(std::forward<TParameters>(parameters)...)
           : nullptr;
   }

   template<typename... TParameters>
   auto make_unique(const key_type& key, TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->make_unique(std::forward<TParameters>(parameters)...)
           : nullptr;
   }

   template<size_t TIndex, typename... TParameters>
   auto make_unique(const key_type& key, TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->template make_unique<TIndex>(std::forward<TParameters>(parameters)...)
           : nullptr;
   }

   /// <summary>Constructs an instance with shared ownership.</summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   auto make_shared(const key_type& key, TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->template make_shared<TInvocable>(std::forward<TParameters>(parameters)...)
           : nullptr;
   }

   template<typename... TParameters>
   auto make_shared(const key_type& key, TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->make_shared(std::forward<TParameters>(parameters)...)
           : nullptr;
   }

   template<size_t TIndex, typename... TParameters>
   auto make_shared(const key_type& key, TParameters&&... parameters) const
   {
      const auto* factory = get(key);

      return factory != nullptr
           ? factory->template make_shared<TIndex>(std::forward<TParameters>(parameters)...)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(factory_map& other) noexcept
   {
      _factories_map.swap(other._factories_map);
   }

private:
   factories_map_type _factories_map;
};
}

