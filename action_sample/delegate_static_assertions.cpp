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

using AA = std::function<int()>;


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
//    static_assert(concepts::invocable::are_all_invocable<functions_t...>,        "At least one of the functions is not invocable.");
    static_assert(concepts::invocable::are_all_different<functions_t...>,        "At least two invocable functions have the same signature.");
    static_assert(concepts::invocable::are_all_same_return_type<functions_t...>, "At least one of the invocable functions doesn't produce the same return type.");

    return 0;
}

int main()
{
    //delegate_t delegate;
    //delegate.invoke<int()>();

    auto a = []()        { return 10;       };
    auto b = [](int x)   { return 10 + x;   };
    auto c = [](float x) { return 10 + x;   };

    auto x = []()        { return 10.0f;    };
    auto y = [](int x)   { return 10.f + x; };
    auto z = [](float x) { return 10.f + x; };

    // Pass
    do_it<A>();
    do_it<X>();

    do_it<decltype(a)>();
    do_it<decltype(x)>();

    do_it<decltype(a), decltype(a)>();

    do_it<B>();
    do_it<Y>();

    do_it<decltype(b)>();
    do_it<decltype(y)>();

    //do_it<A, B>();
    //do_it<X, Y>();

    //do_it<A, B, C>();
    //do_it<X, Y, Z>();

    // Fail
    //do_it<>();
    //do_it<A, A, AA>();
    //do_it<A, B, A>();

    return 0;
}
