#pragma once

#include <echo/k_array2/shape.h>
#include <echo/k_array2/subshape.h>

namespace echo { namespace shape_traits {

using dimensionality_traits::extent_type;
using dimensionality_traits::num_dimensions;
using dimensionality_traits::num_free_dimensions;
using dimensionality_traits::free_dimension;

template<int I, class T>
using stride_type = decltype(get_stride<I>(std::declval<T>()));

}}