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

template<class T>
constexpr auto key_name()
{
    const std::string_view name = typeid(T).name();

    const auto pos = name.find_last_of(' ');

    if (pos == std::string_view::npos)
        return name;

    return name.substr(pos + 1);
}

template<class T>
void register_constructors(api::foo_factory& factory,
                           const api::foo_factory::key_type& key)
{
   factory.register_function(key, std::function<api::nike* ()>(
                                  []()
                                  {
                                     return new T();
                                  }));

   factory.register_function(key, std::function<api::nike* (int, float)>(
                                  [](int a, float b)
                                  {
                                     return new T(a, b);
                                  }));
}


/// <summary>
///  In a scenario in which the the derived class doesn't support all the variations
///  to constructor an instance. Such as the testing::simple_nike class only provides
///  a base constructor without one for numerics.
/// 
///  Therefore, it's possible to register a function that accepts numerics values, but
///  will instead invokes the factory's registered base constructor instead. 
/// </summary>
template<>
void register_constructors<testing::simple_nike>(api::foo_factory& factory,
                                                 const api::foo_factory::key_type& key)
{
   using base_construtor = std::function<api::nike* ()>;
   using numerics_construtor = std::function<api::nike* (int, float)>;

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
        std::cerr << "No methods to run since a null reference was passed in.\n";
        return;
    }

    std::unique_ptr<api::nike> foo_ptr{ nike };

    std::cout << "Running methods.\n";
    foo_ptr->do_it();
}

void test_constructors(const api::foo_factory& factory,
                       const api::foo_factory::key_type& key)
{
   run_methods(factory.construct<std::function<api::nike* ()>>(key));
   run_methods(factory.construct<std::function<api::nike* (int, float)>>(key, 5, 5.0f));

   run_methods(factory.construct<0>(key));
   run_methods(factory.construct<1>(key, 5, 5.0f));
}

void test_factory(api::foo_factory& factory,
                  const api::foo_factory::key_type key)
{
   test_constructors(factory, key);
   factory.unregister_delegate(key);
   test_constructors(factory, key);
}

void configure_application(api::foo_factory& factory)
{
    register_constructors<testing::my_nike>(factory, "my");
    register_constructors<testing::your_nike>(factory, "your");
    register_constructors<testing::simple_nike>(factory, "simple");
    register_constructors<testing::madison>(factory, "madison");
}

int run_application(api::foo_factory& factory)
{
    test_factory(factory, "my");
    test_factory(factory, "your");
    test_factory(factory, "simple");
    test_factory(factory, "madison");

    return 0;
}

int main()
{
   api::foo_factory factory;

   configure_application(factory);

   return run_application(factory);
}
