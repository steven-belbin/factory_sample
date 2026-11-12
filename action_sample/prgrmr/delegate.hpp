#pragma once

#include "./functional.hpp"
#include "./concepts/invocable.hpp"
#include <cstddef>

namespace prgrmr
{
namespace detail
{
   template<typename TInvocable, typename TParametersTuple>
   class delegate_base;

   template<typename TInvocable, typename... TParameters>
   class delegate_base<TInvocable, std::tuple<TParameters...>>
   {
   protected:
      using invocable_type = TInvocable;

      invocable_type _invocable;

      delegate_base() = default;
      delegate_base(const delegate_base&) = delete;
      delegate_base(delegate_base&&) = default;

      ~delegate_base() = default;

      delegate_base& operator=(const delegate_base&) = delete;
      delegate_base& operator=(delegate_base&&) = default;

      explicit delegate_base(const invocable_type& invocable)
      : _invocable{ invocable }
      {
      }

      explicit delegate_base(invocable_type&& invocable)
      : _invocable{ std::forward<invocable_type>(invocable) }
      {
      }

   public:
      ///
      /// <summary>
      ///   Get the invocable.
      /// </summary>
      ///
      /// <returns>A reference to the registered invocable.</returns>
      ///
      /// <remarks>When no invocable has been registered, then reference will be an empty invocable <--> nullptr.</remarks>
      ///
      const auto& invocable() const
      {
        return _invocable;
      }

      ///
      /// <summary>
      ///   Resets the registered invocable to nullptr.
      /// </summary>
      ///
      void reset(std::nullptr_t = nullptr)
      {
         _invocable = nullptr;
      }

      ///
      /// <summary>
      ///   Registers or replaces the invocable.
      /// </summary>
      ///
      /// <param name="invocable">The invocable to register.</param>
      ///
      template<typename F>
      requires std::constructible_from<invocable_type, F>
      void reset(F&& invocable)
      {
         _invocable = std::forward<F>(invocable);
      }

      ///
      /// <summary>
      ///   Invokes the registered invocable with the provided parameters.
      /// </summary>
      ///
      /// <param name="parameters">The parameters to pass to the invocable.</param>
      ///
      auto invoke(TParameters... parameters) const
      {
        return _invocable(std::forward<TParameters>(parameters)...);
      }

      ///
      /// <summary>
      ///   Invokes the registered invocable with the provided parameters.
      /// </summary>
      ///
      /// <param name="parameters">The parameters to pass to the invocable.</param>
      ///
      auto operator()(TParameters... parameters) const
      {
        return invoke(std::forward<TParameters>(parameters)...);
      }

      ///
      /// <summary>
      ///   Invokes the registered invocable with the provided parameters, if it is registered.
      /// </summary>
      ///
      /// <param name="parameters">The parameters to pass to the invocable.</param>
      ///
      /// <returns>
      ///   When the invocable is registered, the result is wrapped in std::optional.
      ///   Otherwise std::nullopt indicates that the invocable isn't registered.
      /// </returns>
      ///
      /// <seealso cref="invoke_optional<function_t>"/>
      ///
      auto invoke_optional(TParameters... parameters) const
      {
        return prgrmr::invoke_optional(_invocable, std::forward<TParameters>(parameters)...);
      }

      ///
      /// <summary>
      ///   Swaps the contents with another reference.
      /// </summary>
      ///
      /// <param name="other">The reference to swap contents with.</param>
      ///
      void swap(delegate_base& other)
      {
          _invocable.swap(other._invocable);
       }

       ///
       /// <summary>
       ///  Checks whether an invocable is registered.
       /// </summary>
       ///
       operator bool() const noexcept
       {
          return static_cast<bool>(_invocable);
       }

       ///
       /// <summary>
       ///  Checks whether an invocable is not registered.
       /// </summary>
       ///
       auto operator!() const noexcept
       {
          return !this->operator bool();
       }
   };
}

///
/// <summary>
///   Contains an invocable that references an invocable or not at runtime.
/// </summary>
///
template<typename TInvocable>
class delegate final
    : private detail::delegate_base<TInvocable, concepts::invocable::invocable_parameters_type<TInvocable>>
{
   using base_type = detail::delegate_base<TInvocable, concepts::invocable::invocable_parameters_type<TInvocable>>;

public:
   using invocable_type = TInvocable;
   using parameters_type = concepts::invocable::invocable_parameters_type<invocable_type>;
   using result_type = concepts::invocable::invocable_result_type<invocable_type>;

   using base_type::invocable;
   using base_type::reset;
   using base_type::invoke;
   using base_type::operator();
   using base_type::invoke_optional;
   using base_type::operator bool;
   using base_type::operator!;

   delegate() = default;
   delegate(const delegate&) = delete;
   delegate(delegate&&) = default;

   ~delegate() = default;

   delegate& operator=(const delegate&) = delete;
   delegate& operator=(delegate&&) = default;

   ///
   /// <summary>
   ///   Construct and registers to an invocable.
   /// </summary>
   ///
   /// <param name="invocable">An invocable.</param>
   ///
   explicit delegate(const invocable_type& invocable)
   : base_type{ invocable }
   {
   }

   ///
   /// <summary>
   ///   Construct and registers to an invocable.
   /// </summary>
   ///
   /// <param name="invocable">An invocable.</param>
   ///
   explicit delegate(invocable_type&& invocable)
   : base_type{ std::forward<invocable_type>(invocable) }
   {
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
       base_type::swap(other);
   }

   ///
   /// <summary>
   ///   Assigns the invocable from another invocable.
   /// </summary>
   ///
   /// <param name="other">The invocable to assign from.</param>
   ///
   template<typename F>
   requires std::constructible_from<invocable_type, F>
   auto& operator=(F&& invocable) noexcept
   {
       reset(std::forward<F>(invocable));
       return *this;
   }

   ///
   /// <summary>
   ///   Unregisters the invocable.
   /// </summary>
   ///
   auto& operator=(const std::nullptr_t)
   {
       reset();
       return *this;
   }
};
}
