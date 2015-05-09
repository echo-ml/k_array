#pragma once

#include <echo/k_array/k_sequence.h>
#include <echo/k_array/shape.h>
#include <echo/k_array/k_shape_strides.h>

namespace echo {
namespace k_array {

///////////////
// KSubshape //
///////////////

template <class Shape, class ShapeStrides>
class KSubshape : public Shape, public ShapeStrides {
 public:
  KSubshape(const Shape& shape, const ShapeStrides& shape_strides)
      : Shape(shape), ShapeStrides(shape_strides) {}

  const auto& shape_strides() const {
    return static_cast<const ShapeStrides&>(shape_strides);
  }
};

template <class Shape, class ShapeStrides>
auto make_k_subshape(const Shape& shape, const ShapeStrides& shape_strides) {
  return KSubshape<Shape, ShapeStrides>(shape, shape_strides);
}

}  // namespace k_array
}  // namespace echo
