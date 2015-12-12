#pragma once

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

namespace prgrmr
{
namespace generic
{
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
   using function_types = std::tuple<decltype(functions_t{})...>;

   delegate_functions() = default;
   ~delegate_functions() = default;

   ///
   /// <summary>
   ///   Constructs an instance that is registered with the given functions.
   /// </summary>
   ///
   /// <param name="functions">All the different function signatures.</param>
   ///
   delegate_functions(const functions_t&... functions)
   : _functions(functions)
   {
   }

   ///
   /// <summary>
   ///   Constructs an instance that is registered with the given functions.
   /// </summary>
   ///
   /// <param name="functions">All the different function signatures.</param>
   ///
   delegate_functions(functions_t&&... functions)
   : _functions(functions)
   {
   }

   ///
   /// <summary>
   ///   Registers all the functions.
   /// </summary>
   ///
   /// <param name="functions">All the different function signatures.</param>
   ///
   void register_functions(const functions_t&... functions)
   {
      _functions = functions;
   }

   ///
   /// <summary>
   ///   Registers all the functions.
   /// </summary>
   ///
   /// <param name="functions">All the different function signatures.</param>
   ///
   void register_functions(functions_t&&... functions)
   {
      _functions.swap(functions);
   }

   ///
   /// <summary>
   ///   Unregisters all the functions.
   /// </summary>
   ///
   void unregister_functions()
   {
      _functions = function_types();
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature.
   /// </summary>
   ///
   template<int index_t>
   auto& get_function()
   {
      return std::get<index_t>(_functions);
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature.
   /// </summary>
   ///
   template<int index_t>
   const auto& get_function() const
   {
      return std::get<index_t>(_functions);
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature.
   /// </summary>
   ///
   template<class function_t>
   auto& get_function()
   {
      return std::get<function_t>(_functions);
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature.
   /// </summary>
   ///
   template<class function_t>
   const auto& get_function() const
   {
      return std::get<function_t>( _functions);
   }

   ///
   /// <summary>
   ///   Registers a single function.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   template<class function_t>
   void register_function(const function_t& function)
   {
      // get_function<function_t>() = function;
      std::get<function_t>(_functions) = function;
   }

   ///
   /// <summary>
   ///   Registers a single function.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   //template<class function_t>
   //void register_function(function_t&& function)
   //{
   //   get_function<function_t>().swap(function);
   //   std::get<function_t>(_functions) = function_t(function);
   //}

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature.
   /// </summary>
   ///
   template<class function_t>
   void unregister_function()
   {
      register_function<function_t>(nullptr);
   }

   ///
   /// <summary>
   ///   Invoke a specific function by its signature.
   /// </summary>
   ///
   /// <param name="args">The function arguments to pass to the function.</param>
   ///
   /// <exception cref="std::bad_function_call">When there is no valid function with this signature which is registered with the given key.</exception>
   ///
   template<class function_t, class... args_t>
   auto invoke(args_t&&... args)
   {
      return get_function<function_t>()(std::forward<args_t>(args)...);
   }
    
private:
   function_types _functions;
};

///
/// <summary>
///   The key_delegates_functions class allows to register a delegate functions.
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
   ~key_delegates_functions() = default;

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           const delegate_type& functions)
   {
      _delegates.emplace(key, functions);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           delegate_type&& functions)
   {
      _delegates.emplace(key, functions);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           functions_t&&... functions)
   {
      register_functions(key, delegate_type(functions));
   }

   ///
   /// <summary>
   ///   Unregisters all the functions that were registered with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the functions were registered under.</param>
   ///
   void unregister_functions(const key_type& key)
   {
      _delegates.erase(key);
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
                          const function_t& function)
   {
      _delegates[key].register_function(function);
   }

   ///
   /// <summary>
   ///   Registers a single function under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="function">The function that is to be registered.</param>
   ///
   //template<class function_t>
   //void register_function(const key_type& key,
   //                       function_t&& function)
   //{
   //   _delegates[key].register_function(function);
   //}

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
      const auto& it = _delegates.find(key);

      if (it != cend(_delegates))
      {
         it->second.unregister_function<function_t>();
      }
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
      const auto& it = _delegates.find(key);

      return (it != cend(_delegates))
             ? it->second.get_function<index_t>()
             : function_t(nullptr);
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
      const auto& it = _delegates.find(key);

      return (it != cend(_delegates))
             ? it->second.get_function<function_t>()
             : function_t(nullptr);
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
   auto invoke(const key_type& key,
               args_t&&... args)
   {
      return _delegates.at(key).invoke<function_t>(std::move(args));
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
   ~key_class_factory() = default;
   
   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           const delegate_type& functions)
   {
      _delegates.register_functions(key, functions);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           delegate_type&& functions)
   {
      _delegates.register_functions(key, functions);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           const functions_t&... functions)
   {
      _delegates.register_functions(key, functions);
   }

   ///
   /// <summary>
   ///   Registers all the functions under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with these function signatures.</param>
   ///
   void register_functions(const key_type& key,
                           functions_t&&... functions)
   {
      _delegates.register_functions(key, functions);
   }

   ///
   /// <summary>
   ///   Unregisters all the functions that were registered with the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key in which the functions were registered under.</param>
   ///
   void unregister_functions(const key_type& key)
   {
      return _delegates.unregister_functions(key);
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
                          const function_t& function)
   {
      _delegates.register_function(key, function);
   }

   ///
   /// <summary>
   ///   Registers a single function under the given key.
   /// </summary>
   ///
   /// <param name="key">The unique identifying key to associate with this function.</param>
   /// <param name="function">The function that is to be registered.</param>
   ///
   //template<class function_t>
   //void register_function(const key_type& key,
   //                       function_t&& function)
   //{
   //   _delegates.register_function(key, function);
   //}

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
   ///   Constructs an instance of the class.
   /// </summary>
   ///
   /// <see cref="key_delegates_functions::invoke"/>
   ///
   /// <returns>An instance of the class.<returns>
   /// <returns>nullptr_t when the given key cannot be found.<returns>
   ///
   template<int index_t>
   auto get_function(const key_type& key) const
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
   template<class function_t>
   auto get_function(const key_type& key) const
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
   template<class function_t, class... args_t>
   typename function_t::result_type construct(const key_type& key,
                                              args_t&&... args) const
   {
      const auto& function = _delegates.get_function<function_t>(key);

      return (function)
             ? function(std::forward<args_t>(args)...)
             : nullptr;
   }

private:
   key_delegates_type _delegates;
};
}
}