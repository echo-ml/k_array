#pragma once

#include <echo/index.h>
#include <echo/concept.h>
#include <echo/contract.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// make_extent
//------------------------------------------------------------------------------
template <index_t I, CONCEPT_REQUIRES(I >= 0)>
auto make_extent(StaticIndex<I> i) {
  return i;
}

inline index_t make_extent(index_t i) {
  CONTRACT_EXPECT { CONTRACT_ASSERT(i >= 0); };
  return i;
}

//------------------------------------------------------------------------------
// extent_type
//------------------------------------------------------------------------------
template <class T>
using extent_type = decltype(make_extent(std::declval<T>()));
}
}
