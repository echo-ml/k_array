#pragma once

#include <echo/k_array/slice.h>
#include <echo/k_array/copy.h>
#include <echo/k_array/k_shape.h>
#include <echo/k_array/k_subshape.h>
#include <echo/k_array/k_array.h>
#include <echo/k_array/k_array_view.h>
#include <echo/k_array/k_array_iteration.h>
#include <echo/k_array/list_form_printer.h>

namespace echo {

using echo::k_array::KShape;
using echo::k_array::KShapeFromExtents;
using echo::k_array::make_k_shape;
using echo::k_array::KSubshape;
using echo::k_array::KArray;
using echo::k_array::KArrayView;
using echo::k_array::is_k_array;
using echo::k_array::KArrayAssignment;
using echo::k_array::get_extent;
using echo::k_array::get_stride;
using echo::k_array::get_num_dimensions;
using echo::k_array::Dimension;
using echo::k_array::Slice;
using echo::k_array::Stride;
using echo::k_array::ListForm;

} //end namespace echo
