# Generic Factory (factory_sample)

A small, header-only C++20 library and samples demonstrating a generic factory pattern that can construct objects (often through an abstract interface) using a flexible set of functors. The factory lets you decouple construction logic from concrete types and supports runtime configuration to decide which constructor/functor to use.

This repository explores two complementary goals:
- Allow many different functors to produce the same abstract product type, even when the concrete object's constructors do not directly support those call patterns.
- Promote configuration-at-runtime to choose which factory functor builds the product, while still enabling certain choices to be bound at compile time for performance or safety.

Contents
- `prgrmr/generic/factory.h` — Core generic factory types and helpers.
- `prgrmr/generic/function_signature_checks.h`, `prgrmr/generic/varadic_type_checks.h` — Compile-time checks used to validate functors and signatures.
- `prgrmr/concepts/concepts.h` — Concept-like utilities and static assertions.
- `action_sample/delegate_static_assertions.cpp` — Small sample showing compile-time signature checks and examples of functors/lambdas.

Why use this
- Build objects through many different call signatures (lambdas, std::function, free functions, bind expressions) without changing the product type.
- Move decision logic out of construction sites: pick the right functor at runtime from configuration (file, environment, command-line).
- When appropriate, bind a functor at compile-time to get the best performance and stronger static guarantees.

How it works (high level)
- The factory holds a registry of functors (callables) that each produce an object conforming to a target interface or concept.
- Functors can have differing parameter lists; helper adapters and compile-time checks normalize and validate signatures.
- At runtime the factory selects and invokes the registered functor. Optionally, some functors can be bound/selected at compile time so other code paths can call them directly.

Usage (typical flow)
1. Define the product interface or target type.
2. Implement or adapt functors (lambdas, std::function objects, function pointers) that produce the product.
3. Register functors with the factory (or configure which functor to use at runtime).
4. Request construction from the factory; the chosen functor is invoked to produce the object.

Example (conceptual)

Samples
- See `action_sample/delegate_static_assertions.cpp` for examples of lambdas, std::function aliases, and compile-time static_assert checks validating sets of callables.

Build
- Open the `action_sample` project in Visual Studio and use __Build__ or __Rebuild Solution__.
- The project is C++20; set the project toolset and language standard to C++20 if required via the project properties.

Notes
- This repo emphasizes safe, compile-time validation of callable sets (see the static assertions in the sample) while allowing flexible runtime selection.
- The implementation is intentionally minimal and header-centric so you can adapt the factory to your codebase and platform.

Contributing
- Fork, add tests or additional adapters, and submit a PR. Keep changes modern C++20 and prefer clear static checks where possible.

License
- See the repository root for licensing information (if present).
