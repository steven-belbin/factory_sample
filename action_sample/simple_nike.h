#include "api_nike.h"
#include "iostream"

namespace testing
{
class simple_nike : virtual public api::nike
{
public:
    simple_nike() = default;
    virtual ~simple_nike() = default;

    virtual void do_it()
    {
        std::cout << "Inside the " << typeid(*this).name() << " class." << std::endl;
    }
};
}
