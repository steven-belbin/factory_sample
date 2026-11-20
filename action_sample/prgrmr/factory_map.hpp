#pragma once

#include "./delegates_map.hpp"
#include "./concepts/arguments.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>
#include <memory>

namespace prgrmr
{
///
/// <summary>
///   A collection of factories that are associated by key.
///
///   Allows applications at runtime to provide a key & configure
///   that key's factory so that application logic can create the
///   appropriate instance that matches key.
/// </summary>
///
/// <seealso cref="delegates_map"/>
///
template<typename TClass, typename TKey, concepts::invocable::Invocable... TInvocables>
requires concepts::arguments::IsNotEmpty<TInvocables...> &&
         (std::same_as<concepts::invocable::invocable_result_type<TInvocables>, TClass*> && ...)
class factory_map final
{
public:
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
   ///   Clears all delegates from the map.
   /// </summary>
   ///
   void clear()
   {
      _delegates_map.clear();
   }

   ///
   /// <summary>
   ///   Checks if the map is empty.
   /// </summary>
   ///
   auto empty() const
   {
      return _delegates_map.empty();
   }

   ///
   /// <summary>
   ///   Gets the number of entries in the map.
   /// </summary>
   ///
   auto size() const
   {
      return _delegates_map.size();
   }

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
   template<concepts::invocable::Invocable... TOtherInvocables>
   requires concepts::arguments::IsNotEmpty<TOtherInvocables...> &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TOtherInvocables...>>>, delegates_type>)
   void reset(const key_type& key,
              TOtherInvocables&&... invocables)
   {
      _delegates_map.reset(key, std::forward<TOtherInvocables>(invocables)...);
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
             !std::same_as<std::remove_cvref_t<TOtherInvocable>, delegates_type>)
   void reset(const key_type& key,
              TOtherInvocable&& invocable)
   {
      _delegates_map.reset(key, std::forward<TOtherInvocable>(invocable));
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
      _delegates_map.reset(key, std::move(invocables_tuple));
   }

   template<concepts::invocable::Invocable... TOtherInvocables>
   requires (sizeof...(TOtherInvocables) == sizeof...(TInvocables))
   void reset(const key_type& key,
              const std::tuple<TOtherInvocables...>& invocables_tuple)
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
   template<size_t TIndex, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TOtherInvocable&& invocable)
   {
      _delegates_map.template
        reset<TIndex>(key, std::forward<TOtherInvocable>(invocable));
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
   template<concepts::invocable::Invocable TInvocable, concepts::invocable::Invocable TOtherInvocable>
   requires (!std::same_as<std::remove_cvref_t<TOtherInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TOtherInvocable&& invocable)
   {
      _delegates_map.template
        reset<TInvocable>(key, std::forward<TOtherInvocable>(invocable));
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
      _delegates_map.template
        reset<TInvocable>(key, nullptr);
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
      _delegates_map.template
        reset<TIndex>(key, nullptr);
   }

   ///
   /// <summary>
   ///   Constructs an instance of the class.
   /// </summary>
   ///
   /// <see cref="key_delegates_functions::invoke"/>
   ///
   /// <returns>An owning pointer, or nullptr if the key or delegate is missing.</returns>
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   TClass* make(const key_type& key,
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
   TClass* make(const key_type& key,
                TParameters&&... parameters) const
   {
      return _delegates_map
             .invoke_optional(key, std::forward<TParameters>(parameters)...)
             .value_or(nullptr);
   }

   ///
   /// <summary>
   ///   Constructs an instance of the class.
   /// </summary>
   ///
   /// <see cref="key_delegates_functions::invoke"/>
   ///
   /// <returns>An owning pointer, or nullptr if the key or delegate is missing.</returns>
   ///
   template<size_t TIndex, typename... TParameters>
   TClass* make(const key_type& key,
                TParameters&&... parameters) const
   {
      return _delegates_map.template
               invoke_optional<TIndex>(key, std::forward<TParameters>(parameters)...)
               .value_or(nullptr);
   }

   /// <summary>Constructs an instance with unique ownership.</summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   std::unique_ptr<TClass> make_unique(const key_type& key, TParameters&&... parameters) const
   {
      return std::unique_ptr<TClass>{ make<TInvocable>(key, std::forward<TParameters>(parameters)...) };
   }

   template<typename... TParameters>
   std::unique_ptr<TClass> make_unique(const key_type& key, TParameters&&... parameters) const
   {
      return std::unique_ptr<TClass>{ make(key, std::forward<TParameters>(parameters)...) };
   }

   template<size_t TIndex, typename... TParameters>
   std::unique_ptr<TClass> make_unique(const key_type& key, TParameters&&... parameters) const
   {
      return std::unique_ptr<TClass>{ make<TIndex>(key, std::forward<TParameters>(parameters)...) };
   }

   /// <summary>Constructs an instance with shared ownership.</summary>
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   std::shared_ptr<TClass> make_shared(const key_type& key, TParameters&&... parameters) const
   {
      return std::shared_ptr<TClass>{ std::unique_ptr<TClass>{ make<TInvocable>(key, std::forward<TParameters>(parameters)...) } };
   }

   template<typename... TParameters>
   std::shared_ptr<TClass> make_shared(const key_type& key, TParameters&&... parameters) const
   {
      return std::shared_ptr<TClass>{ std::unique_ptr<TClass>{ make(key, std::forward<TParameters>(parameters)...) } };
   }

   template<size_t TIndex, typename... TParameters>
   std::shared_ptr<TClass> make_shared(const key_type& key, TParameters&&... parameters) const
   {
      return std::shared_ptr<TClass>{ std::unique_ptr<TClass>{ make<TIndex>(key, std::forward<TParameters>(parameters)...) } };
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
