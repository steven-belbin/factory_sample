#include "bird.h"
#include "jordan.h"
#include "lebron.h"
#include "madison.h"
#include "runner.h"
#include "shoe.h"
#include "shoe_factory.h"
#include <concepts>
#include <iostream>

namespace nike
{
/// <summary>
/// Automatically registers the given class <T> constructors into the
/// class factory for the given key.
/// 
/// If the class <T> has a default constructor, then it will be registered
/// into the factory when invoking without any parameters. Likewise, if
/// the class <T> has a constructor that accepts an int and a float, then
/// then it will be registered as the numerics constructor signature.
/// </summary>
/// <typeparam name="T"></typeparam>
/// <param name="factory"></param>
/// <param name="key"></param>
/// <remarks>
/// This is meant as a convenience function to reduce boilerplate code when
/// the class <T> provides the expected constructors.
/// </remarks>
/// <todo>
/// Implement as a generic function that it supports any factory &
/// derived class from the base type.
///
/// A challenge is that std::make_unique<T, Args...> returns the
/// std::unique_ptr<derived class type> whereas typically the
/// factory's registered constructors return std::unique_ptr<base class type>.
/// </todo>
/// <todo>
/// Implement as a generic function that iterates through the signatures
/// that the factory supports & registers the constructors of class <T>.
///
/// This example is hardcoded for a default initializer & a constructor
/// that accepts an int & a float. However, a more generic approach might
/// reduce boilerplate code when registering classes with many constructors.
/// </todo>
template<class T>
void register_constructors(shoe_factory& factory,
                           const shoe_factory::key_type& key)
{
    std::cout << "============================================================================================\n";
    std::cout << "                         Registering " << key << " constructors.\n";
    std::cout << "============================================================================================\n";

    if constexpr (std::default_initializable<T>)
    {
        factory.register_function<base_constructor>(key, std::make_unique<T>);
        std::cout << "Registered the 'default initializable' constructor.\n";

        if constexpr (!std::constructible_from<T, int, float>)
        {
            auto fun = [&factory, key]([[maybe_unused]] int a, [[maybe_unused]] float b)
                { return factory.construct<base_constructor>(key); };

            factory.register_function<numerics_constructor>(key, std::move(fun));
            std::cout << "Registered an adaptor for the 'numerics' constructor.\n";
        }
    }

    if constexpr (std::constructible_from<T, int, double>)
    {
        factory.register_function<numerics_constructor>(key, std::make_unique<T, int, float>);
        std::cout << "Registered the 'numerics' constructor.\n";

        if constexpr (!std::default_initializable<T>)
        {
            auto fun = [&factory, key]()
                { return factory.construct<numerics_constructor>(key, 0, 0.0f); };

            factory.register_function<base_constructor>(key, std::move(fun));
            std::cout << "Registered an adaptor for the 'default initializable' constructor.\n";
        }
    }
}

/// <summary>
///  Let's override the registeration of 'runner' derived class.
/// </summary>
///
/// <remarks>
///  It doesn't provide a constructor for numerics therefore provide a lambda that
///  is the same signature as the numerics constructor that simply invokes the
///  factory's base constructor to create the instance.
template<>
void register_constructors<runner>(shoe_factory& factory,
                                   const shoe_factory::key_type& key)
{
   factory.register_function(key, base_constructor(
                                  []()
                                  {
                                     return std::make_unique<runner>();
                                  }));

   factory.register_function(key, numerics_constructor(
                                  [&factory, key](int a, float b)
                                  {
                                     return factory.construct<base_constructor>(key);
                                  }));
}

/// <summary>
/// Configure the `nike shoe factory` by registers all the derived
/// `nike shoe` classes into the given factory.
/// </summary>
/// <param name="factory"></param>
inline void configure_shoe_factory(shoe_factory& factory)
{
    register_constructors<bird>(factory, "bird");
    register_constructors<jordan>(factory, "jordan");
    register_constructors<lebron>(factory, "lebron");
    register_constructors<madison>(factory, "madison");
    register_constructors<runner>(factory, "runner");
}

}
