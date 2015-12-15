#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace prgrmr
{
namespace generic
{

template<class function_t>
function_t build_functions_type(function_t&& function)
{
   return std::move(function);
}

template<class... functions_t>
std::tuple<functions_t...> build_functions_type(functions_t&&... functions)
{
   return std::make_tuple(build_functions_type<functions_t>(functions)...);
}

///
/// <summary>
///   The delegate_functions class is a function delegate that supports multiple functions but which different
///   number of arguments or different argument types.
///   <para>When there are many different functions, then they must return the same type.</para>
/// </summary>
///
/// <remarks>to do : add a compile-time check to ensure that each function returns the same type.</remarks>
/// <remarks>to do : add a compile-time check to prevent multiple occurrences of the same function signature.</remarks>
///
template<class... functions_t>
class delegate_functions final
{
public:
   using functions_type = std::tuple<decltype(functions_t{})...>;

   delegate_functions() = default;
   delegate_functions(const delegate_functions&) = default;
   delegate_functions(delegate_functions&&) = delete;

   ~delegate_functions() = default;

   delegate_functions& operator=(const delegate_functions&) = default;
   delegate_functions& operator=(delegate_functions&&) = delete;

   template<class function_t>
   delegate_functions(function_t&& function)
   {
      std::get<function_t>(_functions) = std::move(function);
   }

   template<class function_t, class... functions_t>
   delegate_functions(function_t&& function, functions_t&&... functions)
   {
      std::get<function_t>(_functions) = std::move(function);
      delegate_functions<function_t>(functions...);
   }

   /////
   ///// <summary>
   /////   Constructs an instance and registers all possible functions.
   ///// </summary>
   /////
   ///// <param name="functions">A container of all possible functions.</param>
   /////
   delegate_functions(functions_t&&... functions)
   : _functions(build_functions_type(functions))
   {
   }

   ///
   /// <summary>
   ///   Constructs an instance and registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   //delegate_functions(functions_t&&... functions)
   //: _functions(functions)
   //{
   //}

   ///
   /// <summary>
   ///   Constructs an instance and registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   //delegate_functions(const functions_type& functions)
   //: _functions(functions)
   //{
   //}

   ///
   /// <summary>
   ///   Constructs an instance and registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   //delegate_functions(functions_type&& functions)
   //: _functions(functions)
   //{
   //}

   ///
   /// <summary>
   ///   Registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   void register_functions(const functions_type& functions)
   {
      _functions = functions;
   }

   ///
   /// <summary>
   ///   Registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   void register_functions(functions_type&& functions)
   {
      _functions.swap(functions);
   }

   ///
   /// <summary>
   ///   Unregisters all possible functions.
   /// </summary>
   ///
   void unregister_functions()
   {
      _functions = functions_type();
   }

   ///
   /// <summary>
   ///   Get a specific function by its tuple signature.
   /// </summary>
   ///
   /// <returns>A reference to the registered function.</returns>
   ///
   /// <remarks>When no function has been registered, then reference will be a function(nullptr).</remarks>
   ///
   template<class function_t>
   decltype(auto) get_function() const
   {
      return std::get<function_t>(_functions);
   }

   ///
   /// <summary>
   ///   Get a specific function by its tuple index.
   /// </summary>
   ///
   /// <returns>A reference to the registered function.</returns>
   ///
   /// <remarks>When no function has been registered, then reference will be a function(nullptr).</remarks>
   ///
   template<int index_t>
   decltype(auto) get_function() const
   {
      return std::get<index_t>(_functions);
   }

   ///
   /// <summary>
   ///   Registers a specific function.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   template<class function_t>
   void register_function(function_t&& function)
   {
      std::get<function_t>(_functions) = std::move(function);
   }

   ///
   /// <summary>
   ///   Registers a specific function by its tuple index.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   /// <remarks>Invoke this method when multiple functions have the same signature.</remarks>
   ///
   template<int index_t, class function_t>
   void register_function(function_t function)
   {
      std::get<index_t>(_functions) = std::move(function);
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature.
   /// </summary>
   ///
   template<class function_t>
   void unregister_function()
   {
      std::get<function_t>(_functions) = nullptr;
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its tuple index.
   /// </summary>
   ///
   template<int index_t>
   void unregister_function()
   {
      std::get<index_t>(_functions) = nullptr;
   }

   ///
   /// <summary>
   ///   Invoke a specific function by its signature.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   /// <exception cref="std::bad_function_call">When the function has not been registered.</exception>
   ///
   template<class function_t, class... args_t>
   decltype(auto) invoke(args_t&&... args)
   {
      return get_function<function_t>()(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Invoke a specific function by its tuple index.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   /// <exception cref="std::bad_function_call">When the function has not been registered.</exception>
   ///
   template<int index_t, class... args_t>
   decltype(auto) invoke(args_t&&... args)
   {
      return get_function<index_t>()(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(delegate_functions& other)
   {
      _functions.swap(other._functions);
   }

private:
   functions_type _functions;
};

///
/// <summary>
///   The key_delegates_functions class allows to register a collection of delegate functions.
///   <para>Each delegate is identified by a unique key identifier.</para>
///   <para>When a delegate is found, it is then possible to invoke the corresponding function that matches the given function signature.</para>
/// </summary>
///
template<class key_t, class... functions_t>
class key_delegates_functions final
{
public:
   typedef key_t key_type;
   using function_types = std::tuple<decltype(functions_t{})...>;
   typedef delegate_functions<functions_t...> delegate_type;
   typedef std::unordered_map<key_type, delegate_type> delegates_type;

   key_delegates_functions() = default;
   key_delegates_functions(const key_delegates_functions&) = default;
   key_delegates_functions(key_delegates_functions&&) = default;

   ~key_delegates_functions() = default;

   key_delegates_functions& operator=(const key_delegates_functions&) = default;
   key_delegates_functions& operator=(key_delegates_functions&&) = default;

   ///
   /// <summary>
   ///   Registers a delegate with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   /// <param name="delegate">The delegate that contains the functions that can be invoked.</param>
   ///
   void register_delegate(const key_type& key,
                          const delegate_type& delegate)
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
                          delegate_type&& delegate)
   {
      _delegates.emplace(key, std::move(delegate));
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
                           function_types functions)
   {
      register_delegate(key, delegate_type(std::move(functions)));
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
                          function_t function)
   {
      _delegates[key].register_function(std::move(function));
   }

   ///
   /// <summary>
   ///   Registers a single function under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="function">The function that is to be registered.</param>
   ///
   template<int index_t, class function_t>
   void register_function(const key_type& key,
                          function_t function)
   {
      _delegates[key].register_function<index_t>(std::move(function));
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
      const auto& delegate_iter = _delegates.find(key);

      if (delegate_iter != cend(_delegates))
      {
         delegate_iter->second.unregister_function<function_t>();
      }
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<int index_t>
   void unregister_function(const key_type& key)
   {
      const auto& delegate_iter = _delegates.find(key);

      if (delegate_iter != cend(_delegates))
      {
         delegate_iter->second.unregister_function<index_t>();
      }
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   decltype(auto) get_delegate(const key_type& key) const
   {
      const auto& delegate_iter = _delegates.find(key);

      return (delegate_iter != std::cend(_delegates))
             ? std::addressof(delegate_iter->second)
            : nullptr;
   }


   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   decltype(auto) get_delegate(const key_type& key)
   {
      const auto& delegate_iter = _delegates.find(key);

      return (delegate_iter != std::cend(_delegates))
             ? std::addressof(delegate_iter->second)
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
      const auto& delegate_iter = _delegates.find(key);

      return (delegate_iter != std::cend(_delegates))
             ? delegate_iter->second.get_function<function_t>()
             : decltype(delegate_iter->second.get_function<function_t>())(nullptr);
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<int index_t>
   auto get_function(const key_type& key) const
   {
      const auto& delegate_iter = _delegates.find(key);

      return (delegate_iter != std::cend(_delegates))
             ? delegate_iter->second.get_function<index_t>()
             : decltype(delegate_iter->second.get_function<index_t>())(nullptr);
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
   /// <seealso cref="delegate_functions::invoke" />
   ///
   template<class function_t, class... args_t>
   decltype(auto) invoke(const key_type& key,
                         args_t&&... args) const
   {
      return at(key).invoke<function_t>(std::move(args));
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
   /// <seealso cref="delegate_functions::invoke" />
   ///
   template<int index_t, class... args_t>
   decltype(auto) invoke(const key_type& key,
                         args_t&&... args) const
   {
      return at(key).invoke<index_t>(std::move(args));
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(key_delegates_functions& other)
   {
      _delegates.swap(other._delegates);
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A const reference to a delegate of functions.</returns>
   ///
   decltype(auto) operator[](const key_type& key) const
   {
      return _delegates[key];
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A reference to a delegate of functions.</returns>
   ///
   decltype(auto) operator[](const key_type& key)
   {
      return _delegates[key];
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A const reference to a delegate of functions.</returns>
   ///
   /// <throws>std::out_of_range if the container does not have an element with the specified key.</throws>
   ///
   decltype(auto) at(const key_type& key) const
   {
      return _delegates,at(key);
   }

   ///
   /// <summary>
   ///   Get a reference to a delegate that matches the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with the delegate.</param>
   ///
   /// <returns>A reference to a delegate of functions.</returns>
   ///
   /// <throws>std::out_of_range if the container does not have an element with the specified key.</throws>
   ///
   decltype(auto) at(const key_type& key)
   {
      return _delegates,at(key);
   }

private:
   delegates_type _delegates;
};

///
/// <summary>
///   The key_class_factory class allows to construct a class instance by a key.
///   <para>The class factory has 0..n entries in which each entry is identified by a unique key.</para>
///   <para>Any given entry can construct a class using any one of functions, each with different number of arguments and different argument types.</para>
/// </summary>
///
/// <seealso cref="key_delegates_functions"/>
///
template<class key_t, class... functions_t>
class key_class_factory final
{
public:
   typedef key_t key_type;
   using function_types = std::tuple<decltype(functions_t{})...>;
   typedef key_delegates_functions<key_type, functions_t...> key_delegates_type;
   typedef typename key_delegates_type::delegate_type delegate_type;

   key_class_factory() = default;
   key_class_factory(const key_class_factory&) = default;
   key_class_factory(key_class_factory&&) = default;

   ~key_class_factory() = default;

   key_class_factory& operator=(const key_class_factory&) = default;
   key_class_factory& operator=(key_class_factory&&) = default;

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_delegate(const key_type& key,
                          const delegate_type& delegate)
   {
      _delegates.register_delegate(key, delegate);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_delegate(const key_type& key,
                          delegate_type&& delegate)
   {
      _delegates.register_delegate(key, delegate);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           function_types functions)
   {
      _delegates.register_delegate(key, delegate_type(std::move(functions)));
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
   template<class function_t>
   void register_function(const key_type& key,
                          function_t function)
   {
      _delegates.register_function(key, std::move(function));
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
      _delegates.unregister_function<function_t>(key);
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature that was registered under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the function was registered under.</param>
   ///
   template<int index_t>
   void unregister_function(const key_type& key)
   {
      _delegates.unregister_function<index_t>(key);
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
   template<class function_t>
   decltype(auto) get_function(const key_type& key) const
   {
      return _delegates.get_function<function_t>(key);
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
   template<int index_t>
   decltype(auto) get_function(const key_type& key) const
   {
      return _delegates.get_function<index_t>(key);
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
                  args_t&&... args) const -> typename function_t::result_type
   {
      const auto& function = _delegates.get_function<function_t>(key);

      return (function)
             ? function(std::forward<args_t>(args)...)
             : nullptr;
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
   template<int index_t, class... args_t>
   auto construct(const key_type& key,
                  args_t&&... args) const
   {
      const auto& function = _delegates.get_function<index_t>(key);

      return (function)
             ? function(std::forward<args_t>(args)...)
             : decltype(function(std::forward<args_t>(args)...))(nullptr);
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(key_class_factory& other)
   {
      _delegates.swap(other._delegates);
   }

private:
   key_delegates_type _delegates;
};
}
}