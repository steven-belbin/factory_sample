#include <prgrmr/concepts/concepts.h>
#include <prgrmr/generic/factory.h>
#include <cassert>
#include <iostream>

using namespace prgrmr;

template<typename ... Invocables>
inline constexpr void static_assert_invocable_concepts()
{
    static_assert(concepts::arguments::IsNotEmpty<Invocables...>, "The list of invocables cannot be empty.");
    static_assert(concepts::invocable::IsDifferentSignatures<Invocables...>, "The list of invocables contains at least one pair that have the same signature.");
    static_assert(concepts::invocable::IsSameResultType<Invocables...>, "The list of invocables contains at least one entry whose result type is different from the rest.");
}

int main()
{
    auto a = [](int x)   { return 10;       };
    auto b = [](int x)   { return 10 + x;   };
    auto c = [](float x) { return 10 + x;   };
    auto d = [](char x)  { return int(x); };

    auto x = []()        { return 10.0f;    };
    auto y = [](int x)   { return 10.f + x; };
    auto z = [](float x) { return 10.f + x; };

    // Doesn't compile since the invocable have the same signature.
    // assert_invocable_concepts<decltype(a), decltype(b)>();

    // Doesn't compile since the invocable have different result types.
    // assert_invocable_concepts<decltype(a), decltype(c)>();

    // Compiles, since the result types are the same & they have different signatures.
    static_assert_invocable_concepts<decltype(a), decltype(d)>();

    // Compiles, since the result types are the same & they have different signatures.
    static_assert_invocable_concepts<decltype(x), decltype(y), decltype(z)>();

    // Test delegate_functions invoke_or_default
    using test_fn1 = std::function<int(int)>;
    using test_fn2 = std::function<int(char)>;
    generic::delegate_functions<test_fn1, test_fn2> delegates;

    // Initially unregistered - should return default (0)
    assert(delegates.invoke_or_default<test_fn1>(5) == 0);
    assert(delegates.invoke_or_default<test_fn2>('a') == 0);
    assert(delegates.invoke_or_default(5) == 0);
    assert(delegates.invoke_or_default('a') == 0);
    assert(delegates.invoke_or_default<0>(5) == 0);
    assert(delegates.invoke_or_default<1>('a') == 0);

    // Register and verify
    delegates.register_function<test_fn1>([](int v) { return v * 2; });
    delegates.register_function<test_fn2>([](char c) { return static_cast<int>(c); });

    assert(delegates.invoke_or_default<test_fn1>(5) == 10);
    assert(delegates.invoke_or_default<test_fn2>('A') == 65);
    assert(delegates.invoke_or_default(5) == 10);
    assert(delegates.invoke_or_default('A') == 65);
    assert(delegates.invoke_or_default<0>(5) == 10);
    assert(delegates.invoke_or_default<1>('A') == 65);

    // Test key_delegates_functions invoke_or_default
    generic::key_delegates_functions<std::string, test_fn1, test_fn2> key_delegates;

    // Missing key - should return default (0)
    assert(key_delegates.invoke_or_default<test_fn1>("missing", 5) == 0);
    assert(key_delegates.invoke_or_default("missing", 5) == 0);
    assert(key_delegates.invoke_or_default<0>("missing", 5) == 0);

    // Register delegate and verify
    key_delegates.register_delegate("active", delegates);
    assert(key_delegates.invoke_or_default<test_fn1>("active", 5) == 10);
    assert(key_delegates.invoke_or_default<test_fn2>("active", 'A') == 65);
    assert(key_delegates.invoke_or_default("active", 5) == 10);
    assert(key_delegates.invoke_or_default("active", 'A') == 65);
    assert(key_delegates.invoke_or_default<0>("active", 5) == 10);
    assert(key_delegates.invoke_or_default<1>("active", 'A') == 65);

    std::cout << "All invoke_or_default assertions passed successfully!\n";

    return 0;
}
