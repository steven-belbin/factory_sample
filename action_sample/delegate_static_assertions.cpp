#include <prgrmr/delegate.hpp>
#include <prgrmr/delegates.hpp>
#include <prgrmr/delegates_map.hpp>
#include <prgrmr/concepts/concepts.hpp>
#include <cassert>
#include <iostream>

using namespace prgrmr;

template<typename ... Invocables>
inline constexpr void static_assert_invocable_concepts()
{
    static_assert(concepts::arguments::IsNotEmpty<Invocables...>, "The list of invocables cannot be empty.");
    static_assert(concepts::invocable::AreInvocablesSignaturesUnique<Invocables...>, "The list of invocables contains at least one pair that have the same signature.");
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
    delegates<test_fn1, test_fn2> delegates;

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

    // Test delegates_map invoke_optional
    delegates_map<std::string, test_fn1, test_fn2> delegates_map;

    // Missing key - should return std::nullopt
    assert(delegates_map.invoke_optional<test_fn1>("missing", 5) == std::nullopt);
    assert(delegates_map.invoke_optional("missing", 5) == std::nullopt);
    assert(delegates_map.invoke_optional<0>("missing", 5) == std::nullopt);

    // Register delegate and verify
    delegates_map.register_delegate("active", delegates);
    assert(delegates_map.invoke_optional<test_fn1>("active", 5) == 10);
    assert(delegates_map.invoke_optional<test_fn2>("active", "abc") == 3);
    assert(delegates_map.invoke_optional("active", 5) == 10);
    assert(delegates_map.invoke_optional("active", std::string("abc")) == 3);
    assert(delegates_map.invoke_optional<0>("active", 5) == 10);
    assert(delegates_map.invoke_optional<1>("active", "abc") == 3);

    // Test delegate reset and invoke_optional
    delegate<test_fn1> single_delegate;
    assert(single_delegate.invoke_optional(5) == std::nullopt);

    single_delegate.reset([](int v) { return v * 3; });
    assert(single_delegate.invoke_optional(5) == 15);

    single_delegate.reset();
    assert(single_delegate.invoke_optional(5) == std::nullopt);

    single_delegate.reset([](int v) { return v * 4; });
    assert(single_delegate.invoke_optional(5) == 20);
    assert(single_delegate.invoke(5) == 20);
    assert(single_delegate(5) == 20);

    single_delegate.reset(nullptr);
    assert(single_delegate.invoke_optional(5) == std::nullopt);

    std::cout << "All invoke_optional assertions passed successfully!\n";

    return 0;
}
