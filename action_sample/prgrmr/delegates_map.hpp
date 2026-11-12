#pragma once

#include "./delegates.hpp"
#include "./concepts/invocable.hpp"
#include <unordered_map>

namespace prgrmr
{
///
/// <summary>
///   The delegates_map class allows to register a collection of delegate functions.
///   <para>Each delegate is identified by a unique key identifier.</para>
///   <para>When a delegate is found, it is then possible to invoke the corresponding function that matches the given function signature.</para>
/// </summary>
///
template<class key_t, class... functions_t>
class delegates_map final
{
public:
   using key_type = key_t;
   using delegates_type = delegates<functions_t...>;
   using delegates_map_type = std::unordered_map<key_type, delegates_type>;

   using function_types = delegates_type::functions_type;
   using result_type = delegates_type::result_type;

   delegates_map() = default;
   delegates_map(const delegates_map&) = default;
   delegates_map(delegates_map&&) = default;

   ~delegates_map() = default;

   delegates_map& operator=(const delegates_map&) = default;
   delegates_map& operator=(delegates_map&&) = default;

   ///
   /// <summary>
   ///   Registers a delegate with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   /// <param name="delegate">The delegate that contains the functions that can be invoked.</param>
   ///
   void register_delegate(const key_type& key,
                          const delegates_type& delegate)
   {
      _delegates.emplace(key, delegate);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_delegate(const key_type& key,
                          delegates_type&& delegate)
   {
      _delegates.emplace(key, std::forward<delegates_type>(delegate));
   }

   ///
   /// <summary>
   ///   Unregisters the delegate functions that was associated with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the functions were registered under.</param>
   ///
   void unregister_delegate(const key_type& key)
   {
      _delegates.erase(key);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           function_types&& functions)
   {
       register_delegate(key, delegates_type{ std::forward<function_types>(functions) });
   }

   ///
   /// <summary>
   ///   Registers a single function under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="function">The function that is to be registered.</param>
   ///
   template<class function_t>
   void register_function(const key_type& key,
                          function_t&& function)
   {
      _delegates[key].register_function(std::forward<function_t>(function));
   }

   ///
   /// <summary>
   ///   Registers a single function under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="function">The function that is to be registered.</param>
   ///
   template<size_t index, class function_t>
   void register_function(const key_type& key,
                          function_t&& function)
   {
      _delegates[key].register_function<index>(std::forward<function_t>(function));
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<class function_t>
   void unregister_function(const key_type& key)
   {
      if (const auto delegate = get_delegate(key); delegate != nullptr)
      {
          delegate->template unregister_function<function_t>();
      }
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<size_t index>
   void unregister_function(const key_type& key)
   {
       if (const auto delegate = get_delegate(key); delegate != nullptr)
       {
           delegate->template unregister_function<index>();
       }
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   auto get_delegate(const key_type& key) const
   {
      const auto& iter = _delegates.find(key);
      
      return (iter != std::end(_delegates))
           ? std::addressof(iter->second)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   auto get_delegate(const key_type& key)
   {
      const auto& iter = _delegates.find(key);
      
      return (iter != std::end(_delegates))
           ? std::addressof(iter->second)
           : nullptr;
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<class function_t>
   auto get_function(const key_type& key) const
   {
      const auto delegate = get_delegate(key);

      return (delegate != nullptr)
           ? delegate->template get_function<function_t>()
           : nullptr;
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<size_t index>
   auto get_function(const key_type& key) const
   {
      const auto delegate = get_delegate(key);

      return (delegate != nullptr)
           ? delegate->template get_function<index>()
           : nullptr;
   }

   ///
   /// <summary>
   ///   Invoke a specific function by its signature that registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   /// <param name="args">The function arguments to pass to the function.</param>
   ///
   /// <exception cref="std::out_of_range">When there is no function registered with the given key.</exception>
   ///
   /// <seealso cref="delegate::invoke" />
   ///
   template<class function_t, class... args_t>
   decltype(auto) invoke(const key_type& key,
                         args_t&&... args) const
   {
      return at(key).template invoke<function_t>(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Invoke a specific function by its signature that registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   /// <param name="args">The function arguments to pass to the function.</param>
   ///
   /// <exception cref="std::out_of_range">When there is no function registered with the given key.</exception>
   ///
   /// <seealso cref="delegate::invoke" />
   ///
   template<size_t index, class... args_t>
   decltype(auto) invoke(const key_type& key,
                         args_t&&... args) const
   {
      return at(key).template invoke<index>(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Invokes a specific function by its signature registered under the given key when available,
   ///   otherwise returns default-constructed result_type.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   /// <param name="args">The function arguments to pass to the function.</param>
   ///
   /// <seealso cref="delegate::invoke_optional" />
   ///
   template<class function_t, class... args_t>
   requires concepts::invocable::InvocableWithResult<function_t, args_t...>
   auto invoke_optional(const key_type& key,
                        args_t&&... args) const
   {
      const auto delegate = get_delegate(key);

      return (delegate != nullptr)
           ? delegate->template invoke_optional<function_t>(std::forward<args_t>(args)...)
           : std::nullopt;
   }

   ///
   /// <summary>
   ///   Invokes the function that matches the provided argument types registered under the given key when available,
   ///   otherwise returns std::nullopt.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   /// <param name="args">The function arguments to pass to the function.</param>
   ///
   /// <seealso cref="delegate::invoke_optional" />
   ///
   template<class... args_t>
   auto invoke_optional(const key_type& key,
                        args_t&&... args) const
   {
       using function_t = concepts::invocable::find_invocable_type<std::tuple<args_t...>, functions_t...>;

       return invoke_optional<function_t>(key, std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Invokes a specific function by its tuple index registered under the given key when available,
   ///   otherwise returns default-constructed result_type.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   /// <param name="args">The function arguments to pass to the function.</param>
   ///
   /// <seealso cref="delegate::invoke_optional" />
   ///
   template<size_t index, class... args_t>
   auto invoke_optional(const key_type& key,
                        args_t&&... args) const
   {
      const auto delegate = get_delegate(key);
      
      return (delegate != nullptr)
           ? delegate->template invoke_optional<index>(std::forward<args_t>(args)...)
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
   ///   Get a reference to a delegate of functions that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A const reference to a delegate of functions.</returns>
   ///
   auto operator[](const key_type& key) const
   {
      return _delegates[key];
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of functions that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A reference to a delegate of functions.</returns>
   ///
   auto operator[](const key_type& key)
   {
      return _delegates[key];
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of functions that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A const reference to a delegate of functions.</returns>
   ///
   /// <throws>std::out_of_range if the container does not have an element with the specified key.</throws>
   ///
   auto at(const key_type& key) const
   {
      return _delegates.at(key);
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate of functions that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A reference to a delegate of functions.</returns>
   ///
   /// <throws>std::out_of_range if the container does not have an element with the specified key.</throws>
   ///
   auto at(const key_type& key)
   {
      return _delegates.at(key);
   }

private:
   delegates_map_type _delegates;
};
}
