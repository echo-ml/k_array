#pragma once

#include <echo/k_array/concept.h>

namespace echo {
namespace k_array {

////////////////////////
// get_dimensionality //
////////////////////////

template <class Node, CONCEPT_REQUIRES(concept::shaped<Node>())>
decltype(auto) get_dimensionality(const Node& node) {
  return node.shape().dimensionality();
}

template <class Node, CONCEPT_REQUIRES(!concept::shaped<Node>() &&
                                       concept::dimensioned<Node>())>
decltype(auto) get_dimensionality(const Node& node) {
  return node.dimensionality();
}

//////////////////////
// get_num_elements //
//////////////////////

template <class Node, CONCEPT_REQUIRES(concept::dimensioned<Node>())>
auto get_num_elements(const Node& node) {
  return get_num_elements(get_dimensionality(node));
}

////////////////
// get_extent //
////////////////

template <int I, class Node, CONCEPT_REQUIRES(concept::dimensioned<Node>())>
auto get_extent(const Node& node)
    -> decltype(get_extent<I>(get_dimensionality(node))) {
  return get_extent<I>(get_dimensionality(node));
}
}
}

////////////////////////
// dimensioned_traits //
////////////////////////

namespace echo { namespace dimensioned_traits {

template <class Node>
using dimensionality_type =
    uncvref_t<decltype(k_array::get_dimensionality(std::declval<Node>()))>;

template <class Node>
constexpr auto num_dimensions() -> decltype(
    dimensionality_traits::num_dimensions<dimensionality_type<Node>>()) {
  return dimensionality_traits::num_dimensions<dimensionality_type<Node>>();
}

template <int I, class Node>
using extent_type =
    dimensionality_traits::extent_type<I, dimensionality_type<Node>>;
}
}
