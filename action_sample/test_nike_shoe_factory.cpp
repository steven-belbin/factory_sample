#include "nike/bird.h"
#include "nike/jordan.h"
#include "nike/lebron.h"
#include "nike/madison.h"
#include "nike/runner.h"
#include "nike/shoe.h"
#include "nike/shoe_factory.h"
#include <concepts>
#include <initializer_list>
#include <iostream>
#include <string_view>
#include <type_traits>
#include <vector>

template<class T>
void register_constructors(nike::shoe_factory& factory,
                           const nike::shoe_factory::key_type& key)
{
    using base     = nike::base_constructor;
    using numerics = nike::numerics_constructor;

    if constexpr (std::default_initializable<T>)
    {
       factory.register_function<base>(key, std::make_unique<T>);
    }

    if constexpr (std::constructible_from<T, int, double>)
    {
       factory.register_function<numerics>(key, std::make_unique<T, int, float>);
    }

    if constexpr (std::default_initializable<T> &&
                  std::constructible_from<T, int, float>)
    {
       ;
    }
    else if constexpr (std::default_initializable<T>)
    {
       factory.register_function<numerics>
       (
        key,
        [&factory, key]([[maybe_unused]] int a, [[maybe_unused]] float b)
           { return factory.construct<base>(key); }
       );
    }
    else if constexpr (std::constructible_from<T, int, float>)
    {
       factory.register_function<base>
       (
        key,
        [&factory, key]()
           { return factory.construct<numerics>(key, 0, 0.0f); }
       );
    }
}

/****
/// <summary>
///  The nike::runner class doesn't provide a constructor with numerics signature.
/// 
///  Thus, it's possible to register a function that accepts the above signature and will
///  forward to the factory's registered base constructor instead.
/// </summary>
/// 
/// <remarks>
///  This example demonstrates the capability of being able to instantiate an object of
///  a given class without having to altering that class in order that it complies
///  with the application's factory.
/// 
///  This case is trivial, but there are situations in which application developers need
///  to deserialize the contents from a JSON object or a database record(s) which is 
///  typically more complex than in the underlying class constructor.
/// 
///  Sometimes developers don't want to incur more dependencies with JSON libraries or
///  database functions within a class, or cannot change the class since it may introduce
///  unwanted dependencies, or the class is from another library or group.
/// 
///  This factory approach is ideal then, since application developers whom simply need
///  a specific instance invoke the appropriate function from the given factory. While,
///  allowing developers to provide a shim for the application that instantiates an
///  instance of the object.
///
///  For instance the nike runner only provides a constructor that doesn't accept any
///  parameters. However, some shoes from the application's factory do take two
///  floating point parameters.
///
///  Show below is that the application is configured at runtime so that second
///  signatures, which takes two parameters, is wired as to invoke the factory's
///  function without any parameters. This is called forwarding/chaining the
///  functions. This is akin to how in C++ a class constructor is allow to invoke
///  another of that class' constructors member.
/// </remarks>
template<>
void register_constructors<nike::runner>(nike::shoe_factory& factory,
                                         const nike::shoe_factory::key_type& key)
{
   factory.register_function(key, nike::base_constructor(
                                  []()
                                  {
                                     return new nike::runner();
                                  }));

   factory.register_function(key, nike::numerics_constructor(
                                  [&factory, key](int a, float b)
                                  {
                                     return factory.construct<nike::base_constructor>(key);
                                  }));
}
*******/

void run_shoe_tests(std::unique_ptr<nike::shoe> shoe_ptr)
{
    if (shoe_ptr == nullptr)
    {
       std::cout << "----------  Construction failed.  -------------\n";
       return;
    }

    std::cout << "----------  Construction succeeded & now running the 'do_it' method.  -------------\n";
    shoe_ptr->do_it();
}

void test_constructors(const nike::shoe_factory& factory,
                       const nike::shoe_factory::key_type& key)
{
   std::cout << "----------  Constructing using signatures.  -------------\n";

   std::cout << "Base signature.\n";
   run_shoe_tests(factory.construct<nike::base_constructor>(key));

   std::cout << "\nConstruct using numerics signature.\n";
   run_shoe_tests(factory.construct<nike::numerics_constructor>(key, 5, 5.0f));

   std::cout << "\n----------  Constructing using tuples.  --------------\n";

   std::cout << "Tuple<0>\n";
   run_shoe_tests(factory.construct<0>(key));

   std::cout << "\nTuple<1>\n";
   run_shoe_tests(factory.construct<1>(key, 5, 5.0f));
}

void test_factory(nike::shoe_factory& factory,
                  const nike::shoe_factory::key_type key)
{
   std::cout << "============================================================================================\n";
   std::cout << "                         Constructing " << key << " shoes & running it's tests.\n";
   std::cout << "============================================================================================\n";
   test_constructors(factory, key);

   std::cout << "\nUnregistered all constructors for this key. From this points onwards nothing shall be constructed with this key.\n";
   factory.unregister_delegate(key);

   test_constructors(factory, key);

   std::cout << "\n============================================================================================\n\n";
}

void configure_application(nike::shoe_factory& factory)
{
    register_constructors<nike::bird>   ( factory, "bird"    );
    register_constructors<nike::jordan> ( factory, "jordan"  );
    register_constructors<nike::lebron> ( factory, "lebron"  );
    register_constructors<nike::madison>( factory, "madison" );
    register_constructors<nike::runner> ( factory, "runner"  );
}

int run_application(nike::shoe_factory& factory)
{
    for(const auto& key : { "bird", "jordan", "lebron", "madison", "runner" })
    {
        test_factory(factory, key);
    }

    return 0;
}

//#define ZERO_FUNCTIONS_DELEGATE_TEST

#if defined(ZERO_FUNCTIONS_DELEGATE_TEST)

template<typename ...T>
int check_empty()
{
    //static_assert(sizeof ... (T) >= 1, "Requires at least one argument.");
    //static_assert(prgrmr::concepts::arguments::AtLeast<1, T...>, "Requires at least one argument.");
    //static_assert(prgrmr::concepts::arguments::IsSingle<T...>, "Requires exactly one arguement.");
    //static_assert(prgrmr::concepts::arguments::IsPair<T...>, "Requires exactly two arguements.");
    //static_assert(prgrmr::concepts::arguments::IsEmpty<T...>, "Should have no args.");
    //static_assert(prgrmr::concepts::arguments::IsNotEmpty<T...>, "Should have some args.");

    return 10;
}

//void no_functions_delegate_compiler_check()
//{
//    prgrmr::generic::delegate_functions df;
//
//    df.invoke<void()>();
//}

void different_return_types_functions_delegate_compiler_check()
{
    prgrmr::generic::delegate_functions<std::function<int ()>,
                                        std::function<double (int)>> df;

    df.invoke<void()>();
}

#endif

int main()
{
#if defined(ZERO_FUNCTIONS_DELEGATE_TEST)

   //check_empty<int, float, double>();
   //check_empty<>();

   // no_functions_delegate_compiler_check();

   different_return_types_functions_delegate_compiler_check();

#endif

   nike::shoe_factory factory;

   configure_application(factory);

   return run_application(factory);
}
