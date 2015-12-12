#pragma once

#include <functional>
#include <map>
#include <memory>
#include <string>

namespace prgrmr
{
namespace generic
{
template<class key_t, class result_t, class... arg_t>
class factory final
{
public:
   typedef key_t key_type;
   typedef result_t result_type;
   typedef std::function<result_type (arg_t...)> function_type;

   factory() = default;
   ~factory() = default;

   void register_function(const key_type& key,
                          function_type&& function)
   {
      _functions[key] = function;
   }

   void unregister_function(const key_type& key)
   {
      _functions.erase(key);
   }

   function_type find_function(const key_type& key)
   {
      const auto& it = _functions.find(key);

      return (it != cend(_functions))
         ? it->second
         : function_type();
   }

   result_type create(const key_type& key,
                      arg_t&&... args)
   {
      const auto& function = find_function(key);

      return (function)
         ? function(std::forward<arg_t>(args)...)
         : nullptr;
   }


private:
   std::map<key_type, function_type> _functions;
};
}
}