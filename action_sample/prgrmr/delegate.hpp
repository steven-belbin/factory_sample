#pragma once

#include "./functional.hpp"
#include "./concepts/invocable.hpp"
#include <functional>

namespace prgrmr
{
///
/// <summary>
///   Contains an invocable function which can be registered and unregistered at runtime.
/// </summary>
///
template<class function_t>
class delegate final
{
public:
   using function_type = function_t;
   using result_type = concepts::invocable::invocable_return_t<function_t>;

   delegate() = default;
   delegate(const delegate&) = default;
   delegate(delegate&&) = delete;

   ~delegate() = default;

   delegate& operator=(const delegate&) = default;
   delegate& operator=(delegate&&) = delete;

   ///
   /// <summary>
   ///   Constructs an instance and registers a functions.
   /// </summary>
   ///
   /// <param name="function">An invocable functions.</param>
   ///
   explicit delegate(const function_type& function)
   : _function{ function }
   {
   }

   ///
   /// <summary>
   ///   Constructs an instance and registers a function.
   /// </summary>
   ///
   /// <param name="function">The function to register.</param>
   ///
   explicit delegate(function_type&& function)
   : _function{ std::forward<function_type>(function) }
   {
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
   auto get_function() const
   {
      return _function;
   }

   ///
   /// <summary>
   ///   Registers a specific function.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   void register_function(function_t&& function)
   {
      _function = std::forward<function_t>(function);
   }

   ///
   /// <summary>
   ///   Registers a specific function by its signature.
   /// </summary>
   ///
   /// <param name="function">The function signature that is to be registered.</param>
   ///
   void register_fn(const std::function<function_t>& function)
   {
      _function = function;
   }

   ///
   /// <summary>
   ///   Unregisters the currently registered function.
   /// </summary>
   ///
   void unregister_function()
   {
      _function = nullptr;
   }

   ///
   /// <summary>
   ///   Invokes the registered function with the provided arguments.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   template<class... args_t>
   auto invoke(args_t&&... args) const
   {
       return _function(std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Invokes the registered function with the provided arguments.
   /// </summary>
   ///
   /// <param name="args">The arguments to pass to the function.</param>
   ///
   template<class... args_t>
   auto operator()(args_t&&... args) const
   {
      return invoke(std::forward<args_t>(args)...);
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
       return prgrmr::invoke_optional(_function, std::forward<args_t>(args)...);
   }

   ///
   /// <summary>
   ///   Swaps the contents with another reference.
   /// </summary>
   ///
   /// <param name="other">The reference to swap contents with.</param>
   ///
   void swap(delegate& other)
   {
      _function.swap(other._function);
   }

private:
   function_type _function;
};
}
