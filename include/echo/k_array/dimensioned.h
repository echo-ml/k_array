#pragma once

#include <echo/k_array/concept.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// get_dimensionality
//------------------------------------------------------------------------------
template <class Node, CONCEPT_REQUIRES(concept::shaped<Node>())>
decltype(auto) get_dimensionality(const Node& node) {
  return node.shape().dimensionality();
}

template <class Node, CONCEPT_REQUIRES(!concept::shaped<Node>() &&
                                       concept::dimensioned<Node>())>
decltype(auto) get_dimensionality(const Node& node) {
  return node.dimensionality();
}

//------------------------------------------------------------------------------
// get_num_elements
//------------------------------------------------------------------------------
template <class Node, CONCEPT_REQUIRES(concept::dimensioned<Node>())>
auto get_num_elements(const Node& node) {
  return get_num_elements(get_dimensionality(node));
}

//------------------------------------------------------------------------------
// get_extent
//------------------------------------------------------------------------------
template <int I, class Node, CONCEPT_REQUIRES(concept::dimensioned<Node>())>
auto get_extent(const Node& node)
    -> decltype(get_extent<I>(get_dimensionality(node))) {
  return get_extent<I>(get_dimensionality(node));
}
}
}

namespace echo {
namespace dimensioned_traits {

//------------------------------------------------------------------------------
// dimensionality_type
//------------------------------------------------------------------------------
template <class Node>
using dimensionality_type =
    uncvref_t<decltype(k_array::get_dimensionality(std::declval<Node>()))>;

//------------------------------------------------------------------------------
// num_dimensions
//------------------------------------------------------------------------------
template <class Node, CONCEPT_REQUIRES(k_array::concept::dimensioned<Node>())>
constexpr auto num_dimensions() {
  return dimensionality_traits::num_dimensions<dimensionality_type<Node>>();
}

//------------------------------------------------------------------------------
// extent_type
//------------------------------------------------------------------------------
template <int I, class Node>
using extent_type = decltype(k_array::get_extent<I>(std::declval<Node>()));
}
}

namespace echo {
namespace k_array {
//------------------------------------------------------------------------------
// within_dimensions
//------------------------------------------------------------------------------
template <class Node, CONCEPT_REQUIRES(concept::dimensioned<Node>())>
bool within_dimensions(
    const Node& node,
    const std::array<index_t, dimensioned_traits::num_dimensions<Node>()>&
        point) {
  return within_dimensions(get_dimensionality(node), point);
}
}
}
