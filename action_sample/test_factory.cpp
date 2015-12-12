#include "api_foo.h"

#include "factory.h"

#include <iostream>
#include <typeinfo>

namespace testing
{
class my_foo : virtual public api::foo
{
public:
   my_foo() = default;
   my_foo(int a, float b) : _a(a), _b(b)
   {
   }

   virtual ~my_foo() = default;

   virtual void do_it()
   {
      _a *= 10;
      _b *= 10.0;
      std::cout << "Inside the " << typeid(*this).name() << " class. a: " << _a << ", b: " << _b << std::endl;
   }

private:
   int _a = 1;
   float _b = 1.0;
};

class your_foo : virtual public api::foo
{
public:
   your_foo() = default;
   your_foo(int a, float b) : _a(a), _b(b)
   {
   }

   virtual ~your_foo() = default;

   virtual void do_it()
   {
      _a *= 2;
      _b *= 2.2f;
      std::cout << "Inside the " << typeid(*this).name() << " class. a: " << _a << ", b: " << _b << std::endl;
   }

private:
   int _a = 77;
   float _b = 77.7f;
};

class mock_foo : virtual public api::foo
{
public:
   mock_foo() = default;
   virtual ~mock_foo() = default;

   virtual void do_it()
   {
      std::cout << "Inside the " << typeid(*this).name() << " class." << std::endl;
   }
};
}

template<class T>
void register_constructors(api::foo_factory& factory)
{
   factory.register_function(typeid(T).name(),
                             std::function<api::foo* ()>([]() { return new T(); }));

   factory.register_function(typeid(T).name(),
                             std::function<api::foo* (int, float)>([](int a, float b) { return new T(a, b); }));

   //factory.register_functions(typeid(T).name(),
   //                           []()
   //                           {
   //                              return new T();
   //                           },
   //                           [](int a, float b)
   //                           {
   //                              return new T(a, b);
   //                           });
}

template<>
void register_constructors<testing::mock_foo>(api::foo_factory& factory)
{
   //factory.register_functions(typeid(testing::mock_foo).name(),
   //                           std::function<api::foo* ()>([]() { return new testing::mock_foo(); }),
   //                           std::function<api::foo* (int, float)>([](int, float) { return new testing::mock_foo(); }));

   factory.register_function(typeid(testing::mock_foo).name(),
                             std::function<api::foo* ()>([]() { return new testing::mock_foo(); }));

   factory.register_function(typeid(testing::mock_foo).name(),
                             std::function<api::foo* (int, float)>([](int, float) { return new testing::mock_foo(); }));


   //factory.register_functions(typeid(testing::mock_foo).name(),
   //                           []()
   //                           {
   //                              return new testing::mock_foo();
   //                           },
   //                           [](int a, float b)
   //                           {
   //                              return new testing::mock_foo();
   //                           });
}


template<class T>
void test_factory(api::foo_factory& factory)
{
   const std::string key = typeid(T).name();

//   const auto& factory.get_function<std::function<api::foo* ()>>
   std::unique_ptr<api::foo> yyz(factory.construct<std::function<api::foo* ()>>(key));
   if (yyz != nullptr) yyz->do_it();

   yyz.reset(factory.construct<std::function<api::foo* (int, float)>>(key, 5, 5.0f));
   if (yyz != nullptr) yyz->do_it();

   factory.unregister_functions(key);

   yyz.reset(factory.construct<std::function<api::foo* ()>>(key));
   if (yyz != nullptr) yyz->do_it();

   yyz.reset(factory.construct<std::function<api::foo* (int, float)>>(key, 5, 5.0f));
   if (yyz != nullptr) yyz->do_it();
}

int main()
{
   api::foo_factory factory;
   register_constructors<testing::my_foo>(factory);
   register_constructors<testing::your_foo>(factory);
   register_constructors<testing::mock_foo>(factory);

   test_factory<testing::my_foo>(factory);
   test_factory<testing::your_foo>(factory);
   test_factory<testing::mock_foo>(factory);

   return 0;
}