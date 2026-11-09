#pragma once

#include "./delegates_map.hpp"

namespace prgrmr
{
///
/// <summary>
///   The factory_map class allows to construct a class instance by a key.
///   <para>The class factory has 0..n entries in which each entry is identified by a unique key.</para>
///   <para>Any given entry can construct a class using any one of functions, each with different number of arguments and different argument types.</para>
/// </summary>
///
/// <seealso cref="key_delegates_functions"/>
///
template<class key_t, class... functions_t>
class factory_map final
{
public:
   using delegates_map_type = delegates_map<key_t, functions_t...>;
   using delegates_type = delegates_map_type::delegates_type;
   using key_type = delegates_map_type::key_type;
   using function_types = delegates_map_type::function_types;
   using result_type = delegates_map_type::result_type;

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
   : _delegates{ delegates }
   {
   }

   /// <summary>
   ///   Constructs a factory_map.
   /// </summary>
   ///
   /// <param name="delegate">The delegates.</param>
   ///
   explicit factory_map(delegates_map_type&& delegates)
   : _delegates{ std::forward<delegates_map_type>(delegates) }
   {
   }

   ///
   /// <summary>
   ///   Registers all the delegate under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   /// <param name="delegate">The functions delegate to be registered.</param>
   ///
   void register_delegate(const key_type& key,
                          const delegates_type& delegate)
   {
      _delegates.register_delegate(key, delegate);
   }

   ///
   /// <summary>
   ///   Registers all the delegate under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   /// <param name="delegate">The functions delegate to be registered.</param>
   ///
   void register_delegate(const key_type& key,
                          delegates_type&& delegate)
   {
      _delegates.register_delegate(key, std::forward<delegates_type>(delegate));
   }

   ///
   /// <summary>
   ///   Registers the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   /// <param name="functions">The functions that are to be registered.</param>
   ///
   /// <todo>
   ///   When providing lambda functions this requires explicitly specifying the functions' signature.
   ///      <code>
   ///          factory.register_functions(
   ///              key,
   ///              base_constructor{ []() { return std::make_unique<runner>(); } },
   ///              numeric_constructor{ [&factory, &key](int a, float b) { return factory.construct(key); } }
   ///          );
   ///      </code>
   ///
   ///   It would be useful to code as:
   ///      <code>
   ///          factory.register_function(
   ///              key,
   ///              []() { return std::make_unique<runner>(); }
   ///              [&factory, &key](int a, float b) { return factory.construct(key); }
   ///          );
   ///      </code>
   /// </todo>
   ///
   void register_functions(const key_type& key,
                           function_types&& functions)
   {
      _delegates.register_delegate(key,
                                   delegates_type{ std::forward<function_types>(functions) });
   }

   ///
   /// <summary>
   ///   Registers a specific function by its index position.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   /// <remarks>Invoke this method when multiple functions have the same signature.</remarks>
   ///
   template<size_t index, class function_t>
   void register_function(const key_type& key,
                          function_t&& function)
   {
       _delegates.register_delegate<index>(key, std::forward<function_t>(function));
   }

   ///
   /// <summary>
   ///   Unregisters all the functions that were registered with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the functions were registered under.</param>
   ///
   void unregister_delegate(const key_type& key)
   {
      return _delegates.unregister_delegate(key);
   }

   ///
   /// <summary>
   ///   Registers a single function under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="function">The function that is to be registered.</param>
   ///
   /// <todo>
   ///   When providing a lambda function this requires explicitly specifying the function's signature.
   ///      <code>
   ///          factory.register_function<numeric_constructor>(
   ///              key,
   ///              [](int a, float b) { return std::make_unique<runner>(); }
   ///          );
   ///
   ///          factory.register_function(
   ///              key,
   ///              numeric_constructor{ [&factory, &key](int a, float b) { return factory.construct(key); } }
   ///          );
   ///      </code>
   ///
   ///   It would be useful to code as:
   ///      <code>
   ///          factory.register_function(
   ///              key,
   ///              [&factory, &key](int a, float b) { return factory.construct(key); }
   ///          );
   ///      </code>
   /// </todo>
   ///
   template<class function_t>
   void register_function(const key_type& key,
                          function_t&& function)
   {
      _delegates.register_function(key, std::forward<function_t>(function));
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
      _delegates.template unregister_function<function_t>(key);
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its index position that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<size_t index>
   void unregister_function(const key_type& key)
   {
      _delegates.template unregister_function<index>(key);
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <returns>A reference to the function object.<returns>
   /// <returns>nullptr_t when the given key cannot be found.<returns>
   ///
   template<class function_t>
   auto get_function(const key_type& key) const
   {
      return _delegates.template get_function<function_t>(key);
   }

   ///
   /// <summary>
   ///   Get a specific function by its index position that was registered under the given key.
   /// </summary>
   ///
   /// <returns>A reference to the function object.<returns>
   /// <returns>nullptr_t when the given key cannot be found.<returns>
   ///
   template<size_t index>
   auto get_function(const key_type& key) const
   {
      return _delegates.template get_function<index>(key);
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
   template<class function_t, class... args_t>
   auto construct(const key_type& key,
                  args_t&&... args) const
   {
      return _delegates.template
             invoke_optional<function_t>(key, std::forward<args_t>(args)...)
             .value_or(nullptr);
   }

   ///
   /// <summary>
   ///   Invokes the function that matches the provided argument types.
   /// </summary>
   ///
   template<class... args_t>
   auto construct(const key_type& key,
                  args_t&&... args) const
   {
      return _delegates.invoke_optional(key, std::forward<args_t>(args)...)
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
   template<size_t index, class... args_t>
   auto construct(const key_type& key,
                  args_t&&... args) const
   {
      return _delegates.template
             invoke_optional<index>(key, std::forward<args_t>(args)...)
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
      _delegates.swap(other._delegates);
   }

private:
   delegates_map_type _delegates;
};
}
