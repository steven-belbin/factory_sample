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

    // Test delegate_functions invoke_optional
    using test_fn1 = std::function<int(int)>;
    using test_fn2 = std::function<int(std::string)>;
    generic::delegate_functions<test_fn1, test_fn2> delegates;

    // Initially unregistered - should return std::nullopt
    assert(delegates.invoke_optional<test_fn1>(5) == std::nullopt);
    assert(delegates.invoke_optional<test_fn2>("a") == std::nullopt);
    assert(delegates.invoke_optional(5) == std::nullopt);
    assert(delegates.invoke_optional(std::string("a")) == std::nullopt);
    assert(delegates.invoke_optional<0>(5) == std::nullopt);
    assert(delegates.invoke_optional<1>("a") == std::nullopt);

    // Register and verify
    delegates.register_function<test_fn1>([](int v) { return v * 2; });
    delegates.register_function<test_fn2>([](const std::string& s) { return static_cast<int>(s.length()); });

    assert(delegates.invoke_optional<test_fn1>(5) == 10);
    assert(delegates.invoke_optional<test_fn2>("abc") == 3);
    assert(delegates.invoke_optional(5) == 10);
    assert(delegates.invoke_optional(std::string("abc")) == 3);
    assert(delegates.invoke_optional<0>(5) == 10);
    assert(delegates.invoke_optional<1>("abc") == 3);

    // Test key_delegates_functions invoke_optional
    generic::key_delegates_functions<std::string, test_fn1, test_fn2> key_delegates;

    // Missing key - should return std::nullopt
    assert(key_delegates.invoke_optional<test_fn1>("missing", 5) == std::nullopt);
    assert(key_delegates.invoke_optional("missing", 5) == std::nullopt);
    assert(key_delegates.invoke_optional<0>("missing", 5) == std::nullopt);

    // Register delegate and verify
    key_delegates.register_delegate("active", delegates);
    assert(key_delegates.invoke_optional<test_fn1>("active", 5) == 10);
    assert(key_delegates.invoke_optional<test_fn2>("active", "abc") == 3);
    assert(key_delegates.invoke_optional("active", 5) == 10);
    assert(key_delegates.invoke_optional("active", std::string("abc")) == 3);
    assert(key_delegates.invoke_optional<0>("active", 5) == 10);
    assert(key_delegates.invoke_optional<1>("active", "abc") == 3);

    std::cout << "All invoke_optional assertions passed successfully!\n";

    return 0;
}
