#pragma once

#include "factory.h"

#include <memory>
#include <string>

namespace api
{
class foo
{
public:
   foo() = default;
   virtual ~foo() = 0 {};
   virtual void do_it() = 0;

private:
   foo(const foo&) = delete;
   foo& operator=(const foo&) = delete;
};

class foo_factory final
{
public:
   typedef std::string key_type;
   typedef prgrmr::generic::factory<key_type, foo*> f1_type;
   typedef prgrmr::generic::factory<key_type, foo*, int, float> f2_type;

   foo_factory() = default;
   ~foo_factory() = default;

   void register_functions(const key_type& key,
                           f1_type::function_type&& fn1,
                           f2_type::function_type&& fn2)
   {
      f1.register_function(key, std::move(fn1));
      f2.register_function(key, std::move(fn2));
   }

   void unregister_functions(const key_type& key)
   {
      f1.unregister_function(key);
      f2.unregister_function(key);
   }

   f1_type f1;
   f2_type f2;
};
}
