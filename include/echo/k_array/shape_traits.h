#pragma once

#include <echo/k_array/shape.h>
#include <echo/k_array/subshape.h>

namespace echo { namespace shape_traits {

using dimensionality_traits::extent_type;
using dimensionality_traits::num_dimensions;

template<int I, class T>
using stride_type = decltype(k_array::get_stride<I>(std::declval<T>()));

template <class Shape>
using dimensionality_type =
    uncvref_t<decltype(std::declval<Shape>().dimensionality())>;

template<class Shape, CONCEPT_REQUIRES(k_array::concept::shape<Shape>())>
constexpr int num_free_dimensions() {
  return dimensionality_traits::num_free_dimensions<
    dimensionality_type<Shape>>();
}

template <class Shape, CONCEPT_REQUIRES(num_free_dimensions<Shape>() == 1)>
constexpr int free_dimension() {
  return dimensionality_traits::free_dimension<dimensionality_type<Shape>>();
}

}}
