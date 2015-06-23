#pragma once

#include <echo/k_array/slice.h>
#include <echo/k_array/dimensionality.h>
#include <echo/k_array/shape.h>
#include <echo/k_array/subshape.h>
#include <echo/k_array/k_array.h>
#include <echo/k_array/k_array_view.h>

namespace echo {

using echo::k_array::make_dimensionality;
using echo::k_array::make_shape;
using echo::k_array::make_subshape;
using echo::k_array::Dimensionality;
using echo::k_array::DimensionalityC;
using echo::k_array::Shape;
using echo::k_array::ShapeC;
using echo::k_array::Subshape;
using echo::k_array::KArray;
using echo::k_array::KArrayView;
using echo::k_array::get_extent;
using echo::k_array::get_stride;

}  // end namespace echo
