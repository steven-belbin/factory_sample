#pragma once

#include "shoe.h"
#include <prgrmr/generic/factory.h>
#include <functional>
#include <memory>
#include <string>

namespace nike
{
using base_constructor     = std::function<std::unique_ptr<shoe> ()>;
using numerics_constructor = std::function<std::unique_ptr<shoe> (int, float)>;

/// <summary>
/// This factory can construct a `nike::shoe` abstract base class instance on
/// behalf of applications.
/// </summary>
///
/// <remarks>
/// Typical usage is that an application's main entry point or bootstrapping
/// code is the only code that is aware of these derived classes since it
/// proceeds to register them into the factory.
/// 
/// When the application code needs an instance, it provides the appropriate
/// key that is associated to that derived class along with the parameters
/// to provide to construct a new instance.
/// </remarks>
/// 
/// <remarks>
/// Benefits with this approach are:
/// * The application code is decoupled such that additional derived classes
///   can be implemented without having to rebuild the rest of the application
///   code, with possible the exclusion of the bootstrapping code.
///
/// * The factory could have dependency injection capabilities to provide a
///   specific set of dependencies to the constructed class instances.
///
/// * The factory could be composed of other factories, such custom shoe laces
///   to allow having customized shoes laces for each shoe instance constructed.
///   Those factories are configured the application's bootstrapping.
///
/// * Ability to adapt/shim classes that cannot be modified, such as legacy or
///   third-party classes that don't provide the appropriate constructors.
///
/// * A mock factory can be implemented for unit testing purposes that does
///   specific behaviors when invoking the class instance methods.
/// </remarks>
using shoe_factory =
      prgrmr::generic::key_class_factory<std::string,
                                         base_constructor,
                                         numerics_constructor>;
}
