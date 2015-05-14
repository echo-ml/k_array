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
    return static_cast<const ShapeStrides&>(*this);
  }
};

/////////////////////
// make_k_subshape //
/////////////////////

template <class Shape, class ShapeStrides>
auto make_k_subshape(const Shape& shape, const ShapeStrides& shape_strides) {
  return KSubshape<Shape, ShapeStrides>(shape, shape_strides);
}

//////////////////////
// get_extent_shape //
//////////////////////

template<class Shape, class ShapeStrides>
const auto& get_extent_shape(const KSubshape<Shape, ShapeStrides>& k_subshape) {
  return static_cast<const Shape&>(k_subshape);
}

}  // namespace k_array
}  // namespace echo
