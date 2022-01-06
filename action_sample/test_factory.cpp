#include "api_nike.h"
#include "madison_nike.h"
#include "my_nike.h"
#include "simple_nike.h"
#include "your_nike.h"

#include "factory.h"

#include <functional>
#include <iostream>
#include <typeinfo>
#include <string_view>

using base_construtor = std::function<api::nike* ()>;
using numerics_construtor = std::function<api::nike* (int, float)>;

template<class T>
void register_constructors(api::nike_factory& factory,
                           const api::nike_factory::key_type& key)
{
   factory.register_function(key, base_construtor(
                                  []()
                                  {
                                     return new T();
                                  }));

   factory.register_function(key, numerics_construtor(
                                  [](int a, float b)
                                  {
                                     return new T(a, b);
                                  }));
}


/// <summary>
///  The testing::simple_nike class doesn't provide a constructor with numerics signature.
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
///  While the numerics constructor is trivial, there are situations in which application
///  developers would like to construct an object from a serialized storage such as JSON
///  or even a database. Here a developper could register functions that accepts a JSON
///  body or database connection and deserializes the contents then forwards to the best
///  function in the factory that has been registered.
/// </remarks>
template<>
void register_constructors<testing::simple_nike>(api::nike_factory& factory,
                                                 const api::nike_factory::key_type& key)
{
   factory.register_function(key, base_construtor(
                                  []()
                                  {
                                     return new testing::simple_nike();
                                  }));

   factory.register_function(key, numerics_construtor(
                                  [&factory, key](int a, float b)
                                  {
                                     return factory.get_function<base_construtor>(key)();
                                  }));
}

void run_methods(api::nike* nike)
{
    if (nike == nullptr)
    {
        std::cerr << "No methods to run since a null reference was provided.\n";
        return;
    }

    std::unique_ptr<api::nike> nike_ptr{ nike };

    std::cout << "Running methods.\n";
    nike_ptr->do_it();
}

void test_constructors(const api::nike_factory& factory,
                       const api::nike_factory::key_type& key)
{
   run_methods(factory.construct<std::function<api::nike* ()>>(key));
   run_methods(factory.construct<std::function<api::nike* (int, float)>>(key, 5, 5.0f));

   run_methods(factory.construct<0>(key));
   run_methods(factory.construct<1>(key, 5, 5.0f));
}

void test_factory(api::nike_factory& factory,
                  const api::nike_factory::key_type key)
{
   std::cout << "Constructing objects & running thier methods for the given key.\n";
   test_constructors(factory, key);

   std::cout << "Unregistered all functions for the given key. Nothing shall be constructed after this point with this key.\n";
   factory.unregister_delegate(key);
   test_constructors(factory, key);
}

void configure_application(api::nike_factory& factory)
{
    register_constructors<testing::my_nike>(factory, "my");
    register_constructors<testing::your_nike>(factory, "your");
    register_constructors<testing::simple_nike>(factory, "simple");
    register_constructors<testing::madison>(factory, "madison");
}

int run_application(api::nike_factory& factory)
{
    test_factory(factory, "my");
    test_factory(factory, "your");
    test_factory(factory, "simple");
    test_factory(factory, "madison");

    return 0;
}

int main()
{
   api::nike_factory factory;

   configure_application(factory);

   return run_application(factory);
}
