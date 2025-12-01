#pragma once

#include "../traits/invocable.h"
#include <concepts>

namespace prgrmr::concepts::invocable
{
///
/// Concept to enforce that a list of invocables have the same result type.
///
template<typename ... Invocables>
concept IsSameResultType = traits::invocable::is_same_result_type<Invocables...>;

///
/// Concept to enforce that a list of invocables have unique signatures (parameter types + return type).
///
template<typename ... Invocables>
concept IsDifferentSignatures = traits::invocable::is_different_signatures<Invocables...>;
}
