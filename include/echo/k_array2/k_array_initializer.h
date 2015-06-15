#pragma once

#include <echo/k_array2/shape_traits.h>
#include <echo/k_array2/dimensioned.h>
#include <echo/utility/initializer_multilist.h>
#include <stdexcept>

namespace echo {
namespace k_array {

/////////////////////////
// InitializationError //
/////////////////////////

struct InitializationError : virtual std::runtime_error {
  InitializationError() : std::runtime_error("InitializationError") {}
};

///////////////////////
// KArrayInitializer //
///////////////////////

template <class Derived, class Scalar, class Shape>
struct KArrayInitializer {
  void initialize(InitializerMultilist<
      Scalar, shape_traits::num_dimensions<Shape>()> values) {
    constexpr auto K = shape_traits::num_dimensions<Shape>();
    auto& derived = static_cast<Derived&>(*this);
    auto extents = initializer::get_extents<Scalar, K>(values);
    if (make_dimensionality(extents) != get_dimensionality(derived))
      throw InitializationError();
    initializer::initialize<Scalar, K>(values, derived);
  }
};

////////////////////////////
// KArrayConstInitializer //
////////////////////////////

template <class Derived, class Scalar, class Shape>
struct KArrayConstInitializer : KArrayInitializer<Derived, Scalar, Shape> {
  void initialize(InitializerMultilist<
      Scalar, shape_traits::num_dimensions<Shape>()> values) const {
    auto mutable_this = const_cast<KArrayConstInitializer*>(this);
    mutable_this->initialize(values);
  }
};
}
}
