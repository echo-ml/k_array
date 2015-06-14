#pragma once

#include <echo/k_array2/concept.h>

namespace echo { namespace k_array {

//////////////////////
// get_num_elements //
//////////////////////

template<class Expression,
  CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_num_elements(const Expression& expression) {
  return get_num_elements(expression.shape());
}

////////////////
// get_extent //
////////////////

template <int I, class Expression,
          CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_extent(const Expression& expression) {
  return get_extent<I>(expression.shape());
}

////////////////
// get_stride //
////////////////

template <int I, class Expression,
          CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_stride(const Expression& expression) {
  return get_stride<I>(expression.shape());
}

}}
