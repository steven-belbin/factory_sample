#include <prgrmr/delegate.hpp>
#include <prgrmr/delegates.hpp>
#include <prgrmr/delegates_map.hpp>
#include <prgrmr/factory.hpp>
#include <prgrmr/factory_map.hpp>
#include <prgrmr/concepts/concepts.hpp>
#include <cassert>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

using namespace prgrmr;

template<typename ... Invocables>
inline constexpr void static_assert_invocable_concepts()
{
    static_assert(concepts::arguments::IsNotEmpty<Invocables...>, "The list of invocables cannot be empty.");
    static_assert(concepts::invocable::AreInvocablesSignaturesUnique<Invocables...>, "The list of invocables contains at least one pair that have the same signature.");
    static_assert(concepts::invocable::IsSameResultType<Invocables...>, "The list of invocables contains at least one entry whose result type is different from the rest.");
}

struct factory_product
{
    explicit factory_product(int value, int& destroyed) : value(value), destroyed(destroyed) {}
    ~factory_product() { ++destroyed; }

    int value;
    int& destroyed;
};

template<typename... TInvocables>
concept valid_product_factory = requires { typename factory_map<factory_product, std::string, TInvocables...>; };

static_assert(valid_product_factory<factory_product*(), factory_product*(int)>);
static_assert(valid_product_factory<std::unique_ptr<factory_product>()>);
static_assert(valid_product_factory<std::shared_ptr<factory_product>()>);
static_assert(!valid_product_factory<>);
static_assert(!valid_product_factory<int*()>);

void test_factory_map()
{
    using default_signature = factory_product*();
    using argument_signature = factory_product*(int, int&);
    using move_signature = factory_product*(std::unique_ptr<int>);
    factory_map<factory_product, std::string, default_signature, argument_signature, move_signature> factory;
    static_assert(std::same_as<decltype(factory.make("active")), factory_product*>);
    static_assert(std::same_as<decltype(factory.make_unique("active")), std::unique_ptr<factory_product>>);
    static_assert(std::same_as<decltype(factory.make_shared("active")), std::shared_ptr<factory_product>>);
    int destroyed = 0;
    int argument = 2;

    assert(factory.make("missing") == nullptr);
    assert(factory.make<default_signature>("missing") == nullptr);
    assert(factory.make<0>("missing") == nullptr);
    assert(!factory.make_unique("missing"));
    assert(!factory.make_unique<default_signature>("missing"));
    assert(!factory.make_unique<0>("missing"));
    assert(factory.make_shared("missing").use_count() == 0);
    assert(factory.make_shared<default_signature>("missing").use_count() == 0);
    assert(factory.make_shared<0>("missing").use_count() == 0);

    factory.reset<default_signature>("active", [&destroyed]() { return new factory_product(1, destroyed); });
    assert(factory.make("active", 3, argument) == nullptr);
    assert(!factory.make_unique<argument_signature>("active", 3, argument));
    assert(factory.make_shared<1>("active", 3, argument).use_count() == 0);
    factory.reset<argument_signature>("active", [&destroyed](int value, int& ref)
    {
        ++ref;
        return new factory_product(value + ref, destroyed);
    });
    factory.reset<move_signature>("active", [&destroyed](std::unique_ptr<int> value)
    {
        return new factory_product(*value, destroyed);
    });

    {
        std::unique_ptr<factory_product> raw(factory.make("active"));
        assert(raw && raw->value == 1);
        std::unique_ptr<factory_product> typed(factory.make<argument_signature>("active", 3, argument));
        assert(typed && typed->value == 6 && argument == 3);
        std::unique_ptr<factory_product> indexed(factory.make<1>("active", 3, argument));
        assert(indexed && indexed->value == 7 && argument == 4);
        assert(factory.make_unique("active", std::make_unique<int>(8))->value == 8);
        assert(factory.make_unique<default_signature>("active")->value == 1);
        assert(factory.make_unique<1>("active", 3, argument)->value == 8 && argument == 5);
        assert(factory.make_shared("active", std::make_unique<int>(9))->value == 9);
        auto shared = factory.make_shared<default_signature>("active");
        auto copy = shared;
        assert(shared && shared->value == 1 && shared.use_count() == 2);
        assert(factory.make_shared<1>("active", 3, argument)->value == 9 && argument == 6);
    }
    assert(destroyed == 9);
    factory.reset<argument_signature>("active");
    assert(factory.make<argument_signature>("active", 3, argument) == nullptr);
    assert(!factory.make_unique<1>("active", 3, argument));
    assert(factory.make_shared("active", 3, argument).use_count() == 0);

    factory.reset<default_signature>("null", []() -> factory_product* { return nullptr; });
    assert(factory.make("null") == nullptr);
    assert(!factory.make_unique("null"));
    assert(factory.make_shared("null").use_count() == 0);

    factory.reset<default_signature>("throws", []() -> factory_product* { throw std::runtime_error("factory failure"); });
    try
    {
        factory.make_shared("throws");
        assert(false);
    }
    catch (const std::runtime_error&)
    {
    }

    // Test factory_map with std::unique_ptr return type
    using unique_signature = std::unique_ptr<factory_product>();
    factory_map<factory_product, std::string, unique_signature> unique_factory;
    unique_factory.reset<unique_signature>("active", [&destroyed]() {
        return std::make_unique<factory_product>(42, destroyed);
    });
    static_assert(std::same_as<decltype(unique_factory.make("active")), std::unique_ptr<factory_product>>);
    static_assert(std::same_as<decltype(unique_factory.make_unique("active")), std::unique_ptr<factory_product>>);
    static_assert(std::same_as<decltype(unique_factory.make_shared("active")), std::shared_ptr<factory_product>>);
    {
        auto u = unique_factory.make("active");
        assert(u != nullptr && u->value == 42);
        auto u2 = unique_factory.make_unique("active");
        assert(u2 != nullptr && u2->value == 42);
        auto s = unique_factory.make_shared("active");
        assert(s != nullptr && s->value == 42);
    }

    // Test factory_map with std::shared_ptr return type
    using shared_signature = std::shared_ptr<factory_product>();
    factory_map<factory_product, std::string, shared_signature> shared_factory;
    shared_factory.reset<shared_signature>("active", [&destroyed]() {
        return std::make_shared<factory_product>(84, destroyed);
    });
    static_assert(std::same_as<decltype(shared_factory.make("active")), std::shared_ptr<factory_product>>);
    static_assert(std::same_as<decltype(shared_factory.make_shared("active")), std::shared_ptr<factory_product>>);
    {
        auto s = shared_factory.make("active");
        assert(s != nullptr && s->value == 84);
        auto s2 = shared_factory.make_shared("active");
        assert(s2 != nullptr && s2->value == 84);
    }

    // Test standalone factory (keyless)
    {
        prgrmr::factory<factory_product, default_signature, argument_signature> standalone_factory;
        assert(standalone_factory.make() == nullptr);
        assert(!standalone_factory.make_unique());
        assert(standalone_factory.make_shared().use_count() == 0);

        // Reset with individual lambdas
        standalone_factory.reset(
            [&destroyed]() { return new factory_product(10, destroyed); },
            [&destroyed](int value, int& ref) { ref += value; return new factory_product(value, destroyed); }
        );
        auto p1 = standalone_factory.make();
        assert(p1 != nullptr && p1->value == 10);
        delete p1;

        auto u1 = standalone_factory.make_unique();
        assert(u1 != nullptr && u1->value == 10);

        auto s1 = standalone_factory.make_shared();
        assert(s1 != nullptr && s1->value == 10);

        int arg = 5;
        auto u2 = standalone_factory.make_unique(3, arg);
        assert(u2 != nullptr && u2->value == 3 && arg == 8);

        // Register factory into factory_map via factory_map::reset(key, factory&&)
        factory_map<factory_product, std::string, default_signature, argument_signature> map_from_factory;
        map_from_factory.reset("standalone", std::move(standalone_factory));
        auto u3 = map_from_factory.make_unique("standalone");
        assert(u3 != nullptr && u3->value == 10);

        // Test get and at
        assert(map_from_factory.get("standalone") != nullptr);
        assert(map_from_factory.get("nonexistent") == nullptr);
        assert(map_from_factory.at("standalone").make_unique()->value == 10);
        try
        {
            map_from_factory.at("nonexistent");
            assert(false);
        }
        catch (const std::out_of_range&)
        {
        }
    }
}

int main()
{
    test_factory_map();
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

    // Test delegates reset with parameter pack of lambdas (std::visit style)
    delegates.reset(
        [](int v) { return v * 10; },
        [](const std::string& s) { return static_cast<int>(s.length() * 10); }
    );
    assert(delegates.invoke<test_fn1>(5) == 50);
    assert(delegates.invoke<test_fn2>("abc") == 30);

    // Test parameter pack reset in arbitrary/reverse order
    delegates.reset(
        [](const std::string& s) { return static_cast<int>(s.length() * 100); },
        [](int v) { return v * 100; }
    );
    assert(delegates.invoke<test_fn1>(5) == 500);
    assert(delegates.invoke<test_fn2>("abc") == 300);

    // Restore for remaining tests
    delegates.reset(
        [](int v) { return v * 2; },
        [](const std::string& s) { return static_cast<int>(s.length()); }
    );

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
