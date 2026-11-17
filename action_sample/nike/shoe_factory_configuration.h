#include "bird.h"
#include "jordan.h"
#include "lebron.h"
#include "madison.h"
#include "runner.h"
#include "shoe_factory.h"
#include <concepts>
#include <iostream>

namespace nike
{
///
/// <summary>
/// Automatically registers the given class <TClass> constructors into the
/// class factory for the given key.
/// 
/// If the class <TClass> has a default constructor, then it will be registered
/// into the factory when invoking without any parameters. Likewise, if
/// the class <TClass> has a constructor that accepts an int and a float, then
/// then it will be registered as the numerics constructor signature.
/// </summary>
///
/// <typeparam name="TClass"></typeparam>
/// <param name="factory"></param>
/// <param name="key"></param>
///
/// <remarks>
///   This is meant as a convenience function to reduce boilerplate code when
///   the class <TClass> provides the expected constructors.
/// </remarks>
/// 
template<class TClass>
void configure_shoe_factory(shoe_factory& factory,
                            const shoe_factory::key_type& key)
{
    std::cout << "============================================================================================\n";
    std::cout << "                         Registering " << key << " constructors.\n";
    std::cout << "============================================================================================\n";

    if constexpr (std::default_initializable<TClass> && std::constructible_from<TClass, int, double>)
    {
        factory.reset(key,
          []()
          {
            return std::make_unique<TClass>();
          },
          [](int x, double y)
          {
            return std::make_unique<TClass>(x, y);
          });

        std::cout << "Registered the 'default initializable' & '(int, double)'.\n";
    }
    else if constexpr (std::default_initializable<TClass>)
    {
        factory.reset(key,
          []()
          {
            return std::make_unique<TClass>();
          },
          [&factory, key]([[maybe_unused]] int x, [[maybe_unused]] double y)
          {
            return factory.make(key);
          });

        std::cout << "Registered the 'default initializable' with an adaptor for '(int, double)'.\n";
    }
    else if constexpr (std::constructible_from<TClass, int, double>)
    {
        factory.reset(key,
          [&factory, key]()
          {
            return factory.make(key, 0, 0.0);
          },
          [](int x, double y)
          {
            return std::make_unique<TClass>(x, y);
          });

        std::cout << "Registered the '(int, double)' with an adaptor for the 'default initializable'.\n";
    }
}

///
/// <summary>
///  Demonstrate that it's possible to override the registration of 'runner' derived class.
/// </summary>
///
/// <remarks>
///  The 'runner' derived class only has a default constructor, therefore register
///  its numerics constructor using an lambda adaptor that simply invokes the
///  factory's base constructor for this key.
/// </remarks>
///
template<>
void configure_shoe_factory<runner>(shoe_factory& factory,
                                    const shoe_factory::key_type& key)
{
   std::cout << "============================================================================================\n";
   std::cout << "                         Registering " << key << " constructors.\n";
   std::cout << "============================================================================================\n";

   factory.reset(key,
     []()
     {
        return std::make_unique<runner>();        
     },
     []([maybe_unused] int, [maybe_unused] double)
     {
        return std::make_unique<runner>();
     });
}

///
/// <summary>
/// Configure the `nike shoe factory` by registers all the derived
/// `nike shoe` classes into the given factory.
/// </summary>
///
/// <param name="factory"></param>
///
inline void configure_shoe_factory(shoe_factory& factory)
{
    configure_shoe_factory<bird>(factory, "bird");
    configure_shoe_factory<jordan>(factory, "jordan");
    configure_shoe_factory<lebron>(factory, "lebron");
    configure_shoe_factory<madison>(factory, "madison");
    configure_shoe_factory<runner>(factory, "runner");
}

}
