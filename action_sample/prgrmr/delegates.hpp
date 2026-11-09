#pragma once

#include "./functional.hpp"
#include "./concepts/arguments.hpp"
#include "./concepts/invocable.hpp"
#include <functional>
#include <tuple>
#include <type_traits>

namespace prgrmr
{
///
/// <summary>
///   Contains one to many delegates which can be registered and unregistered at runtime.
/// </summary>
///
template<class... functions_t>
class delegates final
{
public:
   static_assert(concepts::arguments::IsNotEmpty<functions_t...>, "The argument list cannot be empty.");

   static_assert(concepts::invocable::IsSameResultType<functions_t...>,
                 "The argument list contains at least one function doesn't return the same type.");

   static_assert(concepts::invocable::IsDifferentSignatures<functions_t...>,
                 "The argument list contains at least one pair of functions having the same signature.");

   using functions_type = concepts::invocable::invocables_tuple_t<functions_t...>;

   template<typename function_t>
   using stored_type = std::decay_t<function_t>;

   using result_type = concepts::invocable::invocables_result_t<functions_t...>;

   delegates() = default;
   delegates(const delegates&) = default;
   delegates(delegates&&) = delete;

   ~delegates() = default;

   delegates& operator=(const delegates&) = default;
   delegates& operator=(delegates&&) = delete;

   ///
   /// <summary>
   ///   Constructs an instance and registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   explicit delegates(const functions_type& functions)
   : _functions{ functions }
   {
   }

   ///
   /// <summary>
   ///   Constructs an instance and registers all possible functions.
   /// </summary>
   ///
   /// <param name="functions">A container of all possible functions.</param>
   ///
   explicit delegates(functions_type&& functions)
   : _functions{ std::forward<functions_type>(functions) }
   {
   }

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
      _functions = std::forward<functions_type>(functions);
   }

   ///
   /// <summary>
   ///   Unregisters all possible functions.
   /// </summary>
   ///
   void unregister_functions()
   {
      _functions = functions_type{};
   }

   ///
   /// <summary>
   ///   Get a specific function by its signature.
   /// </summary>
   ///
   /// <returns>A reference to the registered function.</returns>
   ///
   /// <remarks>When no function has been registered, then reference will be an empty function <--> nullptr.</remarks>
   ///
   template<class function_t>
   auto get_function() const
   {
      return std::get<stored_type<function_t>>(_functions);
   }

   ///
   /// <summary>
   ///   Get a specific function by its index position.
   /// </summary>
   ///
   /// <returns>A reference to the registered function.</returns>
   ///
   /// <remarks>When no function has been registered, then reference will be an empty function <--> nullptr.</remarks>
   ///
   template<size_t index>
   auto get_function() const
   {
      return std::get<index>(_functions);
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
      std::get<stored_type<function_t>>(_functions) = std::forward<function_t>(function);
   }

   ///
   /// <summary>
   ///   Registers a specific function by its signature.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   template<class function_t>
   void register_fn(const std::function<function_t>& function)
   {
      std::get<stored_type<function_t>>(_functions) = function;
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
   void register_function(function_t&& function)
   {
      std::get<index>(_functions) = std::forward<function_t>(function);
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its signature.
   /// </summary>
   ///
   template<class function_t>
   void unregister_function()
   {
      std::get<stored_type<function_t>>(_functions) = nullptr;
   }

   ///
   /// <summary>
   ///   Unregisters a specific function by its tuple index.
   /// </summary>
   ///
   template<size_t index>
   void unregister_function()
   {
      std::get<index>(_functions) = nullptr;
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
   auto invoke(args_t&&... args)
   {
      return get_function<function_t>()(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Invokes the function that matches the provided argument types.
   /// </summary>
   ///
   template<class... args_t>
   auto invoke(args_t&&... args)
   {
       using function_t = concepts::invocable::find_invocable_t<std::tuple<args_t...>, functions_t...>;

       return invoke<function_t>(std::forward<args_t>(args)...);
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
   template<size_t index, class... args_t>
   auto invoke(args_t&&... args)
   {
      return get_function<index>()(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Locates the function by its signature and when it's registered, invokes it.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   /// <returns>
   ///   When the function is registered, the result is wrapped in std::optional.
   ///   Otherwise std::nullopt indicates that the function isn't registered.
   /// </returns>
   ///
   /// <seealso cref="prgrmr::invoke_optional"/>
   ///
   template<class function_t, class... args_t>
   requires concepts::invocable::InvocableWithResult<function_t, args_t...>
   auto invoke_optional(args_t&&... args) const
   {
      return prgrmr::invoke_optional(get_function<function_t>(),
                                     std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Locates the function by its signature and when it's registered, invokes it.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   /// <returns>
   ///   When the function is registered, the result is wrapped in std::optional.
   ///   Otherwise std::nullopt indicates that the function isn't registered.
   /// </returns>
   ///
   /// <seealso cref="invoke_optional<function_t>"/>
   ///
   template<class... args_t>
   auto invoke_optional(args_t&&... args) const
   {
       using function_t = concepts::invocable::find_invocable_t<std::tuple<args_t...>, functions_t...>;

       return invoke_optional<function_t>(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Locates the function by its tuple index and if one a registered, then invokes it.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   /// <returns>
   ///  An optional containing the result of invoking the function.
   ///  Otherwise, std::nullopt when the invocable function is not defined.
   /// </returns>
   ///
   ///
   /// <seealso cref="prgrmr::invoke_optional"/>
   ///
   template<size_t index, class... args_t>
   auto invoke_optional(args_t&&... args) const
   {
      return prgrmr::invoke_optional(get_function<index>(),
                                     std::forward<args_t>(args)...);
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
      _functions.swap(other._functions);
   }

private:
   functions_type _functions;
};
}
