#pragma once

#include <echo/k_array/shape.h>
#include <echo/k_array/subshape.h>

namespace echo { namespace shape_traits {

using dimensionality_traits::extent_type;
using dimensionality_traits::num_dimensions;
using dimensionality_traits::num_free_dimensions;
using dimensionality_traits::free_dimension;

template<int I, class T>
using stride_type = decltype(k_array::get_stride<I>(std::declval<T>()));

}}
