#include "nike/shoe.h"
#include "nike/shoe_factory.h"
#include "nike/shoe_factory_configuration.h"
#include <iostream>

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

int main()
{
   nike::shoe_factory factory;

   nike::configure_shoe_factory(factory);

   for (const auto& key : { "bird", "jordan", "lebron", "madison", "runner" })
   {
       test_factory(factory, key);
   }

   return 0;
}
