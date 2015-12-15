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

class madison : virtual public api::foo
{
public:
   madison() = default;
   madison(int a, float b) : _a(a), _b(b)
   {
   }

   virtual void do_it()
   {
      for (decltype(_a) i = 0; i < _a; i++)
      {
         std::cout << "madison, i love you!!!" << std::endl;
      }
   }

   int _a = 1;
   float _b = 1.0f;
};
}

template<class T>
void register_constructors(api::foo_factory& factory)
{
   //api::foo_factory::delegate_type delegate
   //(
   //   std::make_tuple
   //   (
   //      std::function<api::foo*()>
   //      (
   //         []()
   //         {
   //            return new T();
   //         }
   //      ),
   //      std::function<api::foo*(int, float)>
   //      (
   //         [](int a, float b)
   //         {
   //            return new T(a, b);
   //         }
   //      )
   //   )
   //);

   const api::foo_factory::key_type key = typeid(T).name();

   //factory.register_delegate(key, delegate);

   factory.register_function(key, std::function<api::foo* ()>([]() -> api::foo* { return new T(); }));
   factory.register_function(key, std::function<api::foo* (int, float)>([](int a, float b) -> api::foo* { return new T(a, b); }));
}

template<>
void register_constructors<testing::mock_foo>(api::foo_factory& factory)
{
   factory.register_delegate(typeid(testing::mock_foo).name(),
                             std::make_tuple(std::function<api::foo* ()>([]() { return new testing::mock_foo(); }),
                                             std::function<api::foo* (int, float)>([](int, float) { return new testing::mock_foo(); })));

   //factory.register_function(typeid(testing::mock_foo).name(),
   //                          std::function<api::foo* ()>([]() { return new testing::mock_foo(); }));

   //factory.register_function(typeid(testing::mock_foo).name(),
   //                          std::function<api::foo* (int, float)>([](int, float) { return new testing::mock_foo(); }));


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

void test_constructors(const api::foo_factory& factory,
                       const api::foo_factory::key_type& key)
{
   std::unique_ptr<api::foo> yyz;

   yyz.reset(factory.construct<std::function<api::foo* ()>>(key));
   if (yyz != nullptr) yyz->do_it();

   yyz.reset(factory.construct<std::function<api::foo* (int, float)>>(key, 5, 5.0f));
   if (yyz != nullptr) yyz->do_it();

   yyz.reset(factory.construct<0>(key));
   if (yyz != nullptr) yyz->do_it();

   yyz.reset(factory.construct<1>(key, 5, 5.0f));
   if (yyz != nullptr) yyz->do_it();
}

template<class T>
void test_factory(api::foo_factory& factory)
{
   const std::string key = typeid(T).name();

   test_constructors(factory, key);
   factory.unregister_delegate(key);
   test_constructors(factory, key);
}

int main()
{
   auto fn = ([](int x) { return x; });

   prgrmr::generic::delegate_functions<std::function<int (int)>,
                                       std::function<char (char)>> delegate_fns;

   delegate_fns.register_function(std::function<int (int)>([](int x) { return x; }));
   delegate_fns.register_function(std::function<char (char)>([](char x) { return x; }));

   int i = 10;
   char c = 'c';

   auto f1a = delegate_fns.get_function<std::function<int (int)>>();
   auto f1b = delegate_fns.get_function<0>();


   const std::string key = "butterfly";

   using kd_type = prgrmr::generic::key_delegates_functions<std::string, std::function<void()>>;

   kd_type kd;
   kd_type::delegate_type del;
   del.register_function<0>([&key]() {std::cout << key << std::endl; });

   kd.register_delegate(key, del);
   decltype(auto) yyz = kd.get_delegate(key);

   if (yyz != nullptr)
   {
      std::cout << "A delegate was found for this key. key:" << key << std::endl;
      yyz->invoke<0>();
   }
   else
   {
      std::cout << "No delegate was found for this key. key:" << key << std::endl;
   }

   std::cout << delegate_fns.invoke<0>(i) << delegate_fns.invoke<1>(c) << std::endl;

   api::foo_factory factory;
   register_constructors<testing::my_foo>(factory);
   register_constructors<testing::your_foo>(factory);
   register_constructors<testing::mock_foo>(factory);
   register_constructors<testing::madison>(factory);

   test_factory<testing::my_foo>(factory);
   test_factory<testing::your_foo>(factory);
   test_factory<testing::mock_foo>(factory);
   test_factory<testing::madison>(factory);

   ///xxx


   return 0;
}