#pragma once

#include <echo/k_array/shape_traits.h>
#include <echo/k_array/concept.h>
#include <echo/k_array/dimensioned.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// get_shape
//------------------------------------------------------------------------------
template <class Node, CONCEPT_REQUIRES(concept::shaped<Node>())>
decltype(auto) get_shape(const Node& node) {
  return node.shape();
}

//------------------------------------------------------------------------------
// get_stride
//------------------------------------------------------------------------------
template <int I, class Node, CONCEPT_REQUIRES(concept::shaped<Node>())>
auto get_stride(const Node& node) {
  return get_stride<I>(get_shape(node));
}
}
}

namespace echo {
namespace shaped_traits {

//------------------------------------------------------------------------------
// dimensionality_type
//------------------------------------------------------------------------------
using dimensioned_traits::dimensionality_type;

//------------------------------------------------------------------------------
// num_dimensions
//------------------------------------------------------------------------------
using dimensioned_traits::num_dimensions;

//------------------------------------------------------------------------------
// extent_type
//------------------------------------------------------------------------------
using dimensioned_traits::extent_type;

//------------------------------------------------------------------------------
// shape_type
//------------------------------------------------------------------------------
template <class Node>
using shape_type =
    uncvref_t<decltype(k_array::get_shape(std::declval<Node>()))>;

//------------------------------------------------------------------------------
// stride_type
//------------------------------------------------------------------------------
// template <int I, class Node>
// using stride_type = shape_traits::stride_type<I, shape_type<Node>>;
template <int I, class Node>
using stride_type = decltype(k_array::get_stride<I>(std::declval<Node>()));
}
}
