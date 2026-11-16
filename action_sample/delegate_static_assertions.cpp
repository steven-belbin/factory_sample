#include <prgrmr/delegate.hpp>
#include <prgrmr/delegates.hpp>
#include <prgrmr/delegates_map.hpp>
#include <prgrmr/concepts/concepts.hpp>
#include <cassert>
#include <iostream>
#include <string>

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

    // Reset (register) and verify
    delegates.reset<test_fn1>([](int v) { return v * 2; });
    delegates.reset<test_fn2>([](const std::string& s) { return static_cast<int>(s.length()); });

    assert(delegates.invoke_optional<test_fn1>(5) == 10);
    assert(delegates.invoke_optional<test_fn2>("abc") == 3);
    assert(delegates.invoke_optional(5) == 10);
    assert(delegates.invoke_optional(std::string("abc")) == 3);
    assert(delegates.invoke_optional<0>(5) == 10);
    assert(delegates.invoke_optional<1>("abc") == 3);

    // Test invoke
    assert(delegates.invoke<test_fn1>(5) == 10);
    assert(delegates.invoke<test_fn2>("abc") == 3);
    assert(delegates.invoke(5) == 10);
    assert(delegates.invoke(std::string("abc")) == 3);
    assert(delegates.invoke<0>(5) == 10);
    assert(delegates.invoke<1>("abc") == 3);

    // Test get
    assert(delegates.get<test_fn1>().invoke(5) == 10);
    assert(delegates.get<int(int)>().invoke(5) == 10);
    assert(delegates.get<0>().invoke(5) == 10);

    // Test reset<TInvocable>() with default nullptr
    delegates.reset<test_fn1>();
    assert(delegates.invoke_optional<test_fn1>(5) == std::nullopt);
    assert(delegates.invoke_optional<test_fn2>("abc") == 3);

    delegates.reset<test_fn1>([](int v) { return v * 2; });
    assert(delegates.invoke_optional<test_fn1>(5) == 10);

    // Test reset<TInvocable>(nullptr)
    delegates.reset<test_fn1>(nullptr);
    assert(delegates.invoke_optional<test_fn1>(5) == std::nullopt);

    delegates.reset<test_fn1>([](int v) { return v * 2; });
    assert(delegates.invoke_optional<test_fn1>(5) == 10);

    // Test reset<index>()
    delegates.reset<0>();
    assert(delegates.invoke_optional<test_fn1>(5) == std::nullopt);
    delegates.reset<0>([](int v) { return v * 2; });
    assert(delegates.invoke_optional<0>(5) == 10);

    // Test non-templated reset() on the whole collection
    delegates.reset();
    assert(delegates.invoke_optional<test_fn1>(5) == std::nullopt);
    assert(delegates.invoke_optional<test_fn2>("abc") == std::nullopt);

    // Re-register for further tests
    delegates.reset<test_fn1>([](int v) { return v * 2; });
    delegates.reset<test_fn2>([](const std::string& s) { return static_cast<int>(s.length()); });

    // Test delegates with function signature types directly
    prgrmr::delegates<int(int), int(std::string)> sig_delegates;
    assert(sig_delegates.invoke_optional<int(int)>(5) == std::nullopt);
    sig_delegates.reset<int(int)>([](int v) { return v * 5; });
    assert(sig_delegates.invoke_optional<int(int)>(5) == 25);
    assert(sig_delegates.invoke<int(int)>(5) == 25);
    assert(sig_delegates.invoke(5) == 25);
    sig_delegates.reset<int(int)>();
    assert(sig_delegates.invoke_optional<int(int)>(5) == std::nullopt);

    // Test delegates with different result types
    prgrmr::delegates<int(int), std::string(int)> diff_result_delegates;
    diff_result_delegates.reset<int(int)>([](int v) { return v * 2; });
    diff_result_delegates.reset<std::string(int)>([](int v) { return std::to_string(v); });
    assert(diff_result_delegates.invoke<int(int)>(5) == 10);
    assert(diff_result_delegates.invoke<std::string(int)>(5) == "5");

    // Test delegates with duplicate signatures (accessed by index)
    prgrmr::delegates<int(int), int(int)> dup_sig_delegates;
    dup_sig_delegates.reset<0>([](int v) { return v + 1; });
    dup_sig_delegates.reset<1>([](int v) { return v + 2; });
    assert(dup_sig_delegates.invoke<0>(5) == 6);
    assert(dup_sig_delegates.invoke<1>(5) == 7);

    // Test delegates_map invoke_optional and reset
    delegates_map<std::string, test_fn1, test_fn2> delegates_map;

    // Missing key - should return std::nullopt
    assert(delegates_map.invoke_optional<test_fn1>("missing", 5) == std::nullopt);
    assert(delegates_map.invoke_optional("missing", 5) == std::nullopt);
    assert(delegates_map.invoke_optional<0>("missing", 5) == std::nullopt);

    // Reset with delegates_type and verify
    delegates_map.reset("active", std::move(delegates));
    assert(delegates_map.invoke_optional<test_fn1>("active", 5) == 10);
    assert(delegates_map.invoke_optional<test_fn2>("active", "abc") == 3);
    assert(delegates_map.invoke_optional("active", 5) == 10);
    assert(delegates_map.invoke_optional("active", std::string("abc")) == 3);
    assert(delegates_map.invoke_optional<0>("active", 5) == 10);
    assert(delegates_map.invoke_optional<1>("active", "abc") == 3);

    // Test resetting with parameter pack of invocables
    delegates_map.reset("pack", [](int v) { return v * 3; }, [](const std::string& s) { return static_cast<int>(s.length() * 2); });
    assert(delegates_map.invoke_optional<test_fn1>("pack", 5) == 15);
    assert(delegates_map.invoke_optional<test_fn2>("pack", "abc") == 6);

    // Test resetting a single invocable matching key
    delegates_map.reset<test_fn1>("pack", [](int v) { return v * 4; });
    assert(delegates_map.invoke_optional<test_fn1>("pack", 5) == 20);

    // Test resetting by index
    delegates_map.reset<0>("pack", [](int v) { return v * 5; });
    assert(delegates_map.invoke_optional<0>("pack", 5) == 25);

    // Test resetting key to nullptr
    delegates_map.reset("pack", nullptr);
    assert(delegates_map.invoke_optional<test_fn1>("pack", 5) == std::nullopt);

    using test_fnx = std::function<int(int)>;

    // Test delegate reset and invoke_optional
    delegate<test_fnx> single_delegate;
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

    // Test delegate<int(int)> with signature type directly
    delegate<int(int)> sig_delegate;
    assert(sig_delegate.invoke_optional(5) == std::nullopt);

    sig_delegate.reset([](int v) { return v * 10; });
    assert(sig_delegate.invoke_optional(5) == 50);
    assert(sig_delegate.invoke(5) == 50);
    assert(sig_delegate(5) == 50);

    //sig_delegate.reset();
    sig_delegate = nullptr;
    assert(sig_delegate.invoke_optional(5) == std::nullopt);

    // Test delegate with constructor taking a lambda
    delegate<int(int)> init_delegate([](int v) { return v + 1; });
    assert(init_delegate.invoke(5) == 6);
    assert(init_delegate(5) == 6);
    assert(init_delegate.invoke_optional(5) == 6);

    std::cout << "All invoke_optional assertions passed successfully!\n";

    return 0;
}
