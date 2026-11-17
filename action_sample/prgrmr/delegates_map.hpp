#pragma once

#include "./delegates.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>
#include <unordered_map>

namespace prgrmr
{
///
/// <summary>
///   The delegates_map class allows to register a collection of delegates.
///   <para>Each delegate is identified by a unique key identifier.</para>
///   <para>When a delegate is found, it is then possible to invoke the corresponding invocable that matches the given function signature.</para>
/// </summary>
///
template<typename TKey, concepts::invocable::Invocable... TInvocables>
class delegates_map final
{
public:
   using key_type = TKey;
   using delegates_type = delegates<TInvocables...>;
   using delegates_map_type = std::unordered_map<key_type, delegates_type>;

   delegates_map() = default;
   delegates_map(const delegates_map&) = default;
   delegates_map(delegates_map&&) = default;

   ~delegates_map() = default;

   delegates_map& operator=(const delegates_map&) = default;
   delegates_map& operator=(delegates_map&&) = default;

   ///
   /// <summary>
   ///   Clears all delegates from the map.
   /// </summary>
   ///
   void clear()
   {
      _delegates.clear();
   }

   ///
   /// <summary>
   ///   Checks if the map is empty.
   /// </summary>
   ///
   auto empty() const
   {
      return _delegates.empty();
   }

   ///
   /// <summary>
   ///   Provides the number of entries in the map.
   /// </summary>
   ///
   auto size() const
   {
      return _delegates.size();
   }

   ///
   /// <summary>
   ///   Erases an entry  from the map that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegates.</param>
   ///
   void erase(const key_type& key)
   {
      _delegates.erase(key);
   }

   ///
   /// <summary>
   ///   Registers all the invocables under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void reset(const key_type& key,
              delegates_type&& delegates)
   {
      _delegates.insert_or_assign(key, std::move(delegates));
   }

   ///
   /// <summary>
   ///   Unregisters the delegate of invocables that was associated with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the functions were registered under.</param>
   ///
   void reset(const key_type& key, std::nullptr_t = nullptr)
   {
      if (const auto delegate = get(key); delegate != nullptr)
      {
         delegate->reset();
      }
   }

   ///
   /// <summary>
   ///   Registers a new delegate instance with these invocables under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   /// <param name="invocables">The invocables that are to be registered as delegate under this key.</param>
   ///
   template<typename... TArgs>
   requires (sizeof...(TInvocables) > 1 && sizeof...(TArgs) == sizeof...(TInvocables)) &&
            (!std::same_as<std::remove_cvref_t<std::tuple_element_t<0, std::tuple<TArgs...>>>, delegates_type>)
   void reset(const key_type& key,
              TArgs&&... invocables)
   {
      _delegates.insert_or_assign(key, delegates_type{ std::forward<TArgs>(invocables)... });
   }

   ///
   /// <summary>
   ///   Resets the delegate matching the provided invocable under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key.</param>
   /// <param name="invocable">The invocable to register.</param>
   ///
   template<typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t> &&
             !std::same_as<std::remove_cvref_t<TTargetInvocable>, delegates_type>)
   void reset(const key_type& key,
              TTargetInvocable&& invocable)
   {
      constexpr std::size_t index = concepts::invocable::detail::find_matching_delegate_index<TTargetInvocable, TInvocables...>();
      reset<index>(key, std::forward<TTargetInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Registers a new delegate instance with a tuple of invocables under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key.</param>
   /// <param name="invocables_tuple">The tuple of invocables.</param>
   ///
   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   void reset(const key_type& key,
              std::tuple<TArgs...>&& invocables_tuple)
   {
      _delegates.insert_or_assign(key, delegates_type{ std::move(invocables_tuple) });
   }

   template<typename... TArgs>
   requires (sizeof...(TArgs) == sizeof...(TInvocables))
   void reset(const key_type& key,
              const std::tuple<TArgs...>& invocables_tuple)
   {
      _delegates.insert_or_assign(key, delegates_type{ invocables_tuple });
   }

   ///
   /// <summary>
   ///   Resets a single invocable under the given key by its signature.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="invocable">The function that is to be registered.</param>
   ///
   template<concepts::invocable::Invocable TInvocable, typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TTargetInvocable&& invocable)
   {
      _delegates[key].template
      reset<TInvocable>(std::forward<TTargetInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Resets a single invocable under the given key by its index.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="invocable">The function that is to be registered.</param>
   ///
   template<size_t TIndex, typename TTargetInvocable>
   requires (!std::same_as<std::remove_cvref_t<TTargetInvocable>, std::nullptr_t>)
   void reset(const key_type& key,
              TTargetInvocable&& invocable)
   {
      _delegates[key].template
      reset<TIndex>(std::forward<TTargetInvocable>(invocable));
   }

   ///
   /// <summary>
   ///   Unregisters a specific invocable by its signature from the delegate that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   template<concepts::invocable::Invocable TInvocable>
   void reset(const key_type& key,
              std::nullptr_t)
   {
      if (const auto delegate = get(key); delegate != nullptr)
      {
          delegate->template reset<TInvocable>(nullptr);
      }
   }

   ///
   /// <summary>
   ///   Unregisters a specific invocable by its index from the delegate that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   template<size_t TIndex>
   void reset(const key_type& key,
              std::nullptr_t)
   {
      if (const auto delegate = get(key); delegate != nullptr)
      {
          delegate->template reset<TIndex>(nullptr);
      }
   }

   ///
   /// <summary>
   ///   Get a specific invocable by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   auto get(const key_type& key) const
   {
      const auto& iter = _delegates.find(key);
      
      return (iter != std::end(_delegates))
           ? std::addressof(iter->second)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Get a specific invocable by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   auto get(const key_type& key)
   {
      const auto& iter = _delegates.find(key);
      
      return (iter != std::end(_delegates))
           ? std::addressof(iter->second)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Invoke a specific invocable by its signature that registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   /// <param name="parameters">The parameters to pass to the matching delegate.</param>
   ///
   /// <exception cref="std::out_of_range">When there is no function registered with the given key.</exception>
   ///
   /// <seealso cref="delegate::invoke" />
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   decltype(auto) invoke(const key_type& key,
                         TParameters&&... parameters) const
   {
      return at(key).template
             invoke<TInvocable>(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invoke a specific invocable by its signature that registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   /// <param name="parameters">The parameters to pass to the matching delegate.</param>
   ///
   /// <exception cref="std::out_of_range">When there is no function registered with the given key.</exception>
   ///
   /// <seealso cref="delegate::invoke" />
   ///
   template<size_t TIndex, typename... TParameters>
   decltype(auto) invoke(const key_type& key,
                         TParameters&&... parameters) const
   {
      return at(key).template
             invoke<TIndex>(std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invokes a specific invocable by its signature registered under the given key when available,
   ///   otherwise returns std::nullopt.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   /// <param name="parameters">The parameters to pass to the matching delegate.</param>
   ///
   /// <seealso cref="delegate::invoke_optional" />
   ///
   template<concepts::invocable::Invocable TInvocable, typename... TParameters>
   requires concepts::invocable::InvocableWithResult<TInvocable>
   auto invoke_optional(const key_type& key,
                        TParameters&&... parameters) const
   {
      const auto delegate = get(key);

      return (delegate != nullptr)
           ? delegate->template invoke_optional<TInvocable>(std::forward<TParameters>(parameters)...)
           : std::nullopt;
   }

   ///
   /// <summary>
   ///   Invokes the invocable that matches the provided argument types registered under the given key when available,
   ///   otherwise returns std::nullopt.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   /// <param name="parameters">The parameters to pass to the matching delegate.</param>
   ///
   /// <seealso cref="delegate::invoke_optional" />
   ///
   template<typename... TParameters>
   auto invoke_optional(const key_type& key,
                        TParameters&&... parameters) const
   {
       using invocable_type = concepts::invocable::find_invocable_type<std::tuple<TParameters...>, TInvocables...>;

       return invoke_optional<invocable_type>(key, std::forward<TParameters>(parameters)...);
   }

   ///
   /// <summary>
   ///   Invokes a specific invocable by its tuple index registered under the given key when available,
   ///   otherwise returns std::nullopt.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   /// <param name="parameters">The parameters to pass to the matching delegate.</param>
   ///
   /// <seealso cref="delegate::invoke_optional" />
   ///
   template<size_t TIndex, typename... TParameters>
   auto invoke_optional(const key_type& key,
                        TParameters&&... parameters) const
   {
      const auto delegate = get(key);
      
      return (delegate != nullptr)
           ? delegate->template invoke_optional<TIndex>(std::forward<TParameters>(parameters)...)
           : std::nullopt;
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(delegates_map& other)
   {
      _delegates.swap(other._delegates);
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of invocables that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   /// <returns>A const reference to a delegate of functions.</returns>
   ///
   const auto& operator[](const key_type& key) const
   {
      return _delegates.at(key);
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of invocables that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   /// <returns>A reference to the delegates associated to the that key.</returns>
   ///
   auto& operator[](const key_type& key)
   {
      return _delegates[key];
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of invocables that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   /// <returns>A const reference to a delegate of invocables.</returns>
   ///
   /// <throws>std::out_of_range if the container does not have an element with the specified key.</throws>
   ///
   const auto& at(const key_type& key) const
   {
      return _delegates.at(key);
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of invocables that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The key to locate the matching delegates.</param>
   ///
   /// <returns>A reference to the invocables associated to the that key.</returns>
   ///
   /// <throws>std::out_of_range if the container does not have an element with the specified key.</throws>
   ///
   auto& at(const key_type& key)
   {
      return _delegates.at(key);
   }

private:
   delegates_map_type _delegates;
};
}
