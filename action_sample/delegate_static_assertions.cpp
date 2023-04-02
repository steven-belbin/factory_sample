#include <prgrmr/concepts/concepts.h>
#include <prgrmr/generic/factory.h>

using namespace prgrmr;
using namespace prgrmr::generic;

using A = std::function<int ()>;
using B = std::function<int (int)>;
using C = std::function<int (float)>;

using X = std::function<float ()>;
using Y = std::function<float (int)>;
using Z = std::function<float (float)>;

//using delegate_t = delegate_functions<>;
//using delegate_t = delegate_functions<A, X>;
//using delegate_t = delegate_functions<A, B, A>;

template<typename ... functions_t>
int do_it()
{
    //static_assert(concepts::arguments::IsNotEmpty<functions_t...>,           "The list of functions cannot be empty.");
    //static_assert(concepts::invocable::AreAllInvocable<functions_t...>, "At least one of the functions is not invocable.");
    //static_assert(concepts::invocable::AreAllDifferent<functions_t...>,      "At least two invocable functions have the same signature.");
    //static_assert(concepts::invocable::AreAllSameReturnType<functions_t...>, "At least one of the invocable functions doesn't produce the same return type.");

    static_assert(concepts::arguments::is_not_empty<functions_t...>,             "The list of functions cannot be empty.");
    static_assert(concepts::invocable::are_all_invocable<functions_t>,        "At least one of the functions is not invocable.");
    //static_assert(concepts::invocable::are_all_different<functions_t...>,        "At least two invocable functions have the same signature.");
    static_assert(concepts::invocable::are_all_same_return_type<functions_t...>, "At least one of the invocable functions doesn't produce the same return type.");

    return 0;
}

int main()
{
    //delegate_t delegate;
    //delegate.invoke<int()>();

    //do_it<>();
    do_it<A,B>();
    //do_it<A, B, A>();

    return 0;
}
