#include "nike/shoe.h"
#include "nike/jordan.h"
#include "nike/lebron.h"
#include "nike/madison.h"
#include "nike/runner.h"
#include "nike/shoe_factory.h"
#include <initializer_list>
#include <iostream>
#include <string_view>
#include <vector>

template<class T>
void register_constructors(nike::shoe_factory& factory,
                           const nike::shoe_factory::key_type& key)
{
   factory.register_function(key, nike::base_constructor(
                                  []()
                                  {
                                     return new T();
                                  }));

   factory.register_function(key, nike::numerics_constructor(
                                  [](int a, float b)
                                  {
                                     return new T(a, b);
                                  }));
}

/// <summary>
///  The nike::runner class doesn't provide a constructor with numerics signature.
/// 
///  Thus, it's possible to register a function that accepts the above signature and will
///  forward to the factory's registered base constructor instead. 
/// </summary>
/// 
/// <remarks>
///  This is motivating example of being able to construct a class but without having to
///  alter it in order to comply with different means in which the application would need
///  to construct it.
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
///  only a single developer is needed to provide the implementation to construct that
///  instance.
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

void run_methods(nike::shoe* shoe)
{
    std::unique_ptr<nike::shoe> shoe_ptr{ shoe };

    if (shoe_ptr == nullptr)
    {
        std::cerr << "No methods to run since a null reference was provided.\n";
        return;
    }

    std::cout << "Running methods.\n";
    shoe_ptr->do_it();
}

void test_constructors(const nike::shoe_factory& factory,
                       const nike::shoe_factory::key_type& key)
{
   run_methods(factory.construct<nike::base_constructor>(key));
   run_methods(factory.construct<nike::numerics_constructor>(key, 5, 5.0f));

   run_methods(factory.construct<0>(key));
   run_methods(factory.construct<1>(key, 5, 5.0f));
}

void test_factory(nike::shoe_factory& factory,
                  const nike::shoe_factory::key_type key)
{
   std::cout << "Constructing shoes & running thier methods for the given key.\n";
   test_constructors(factory, key);

   std::cout << "Unregistered all functions for the given key. Nothing shall be constructed after this point with this key.\n";
   factory.unregister_delegate(key);
   test_constructors(factory, key);
}

void configure_application(nike::shoe_factory& factory)
{
    register_constructors<nike::jordan>(factory, "jordan");
    register_constructors<nike::lebron>(factory, "lebron");
    register_constructors<nike::runner>(factory, "runner");
    register_constructors<nike::madison>(factory, "madison");
}

int run_application(nike::shoe_factory& factory)
{
    for(const auto& key : { "jordan", "lebron, runner", "madison" })
    {
        test_factory(factory, key);
    }

    return 0;
}

int main()
{
   nike::shoe_factory factory;

   configure_application(factory);

   return run_application(factory);
}
