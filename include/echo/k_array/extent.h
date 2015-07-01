#pragma once

#include <echo/index.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// make_extent
//------------------------------------------------------------------------------
template <index_t I>
auto make_extent(StaticIndex<I> i) {
  return i;
}

inline index_t make_extent(index_t i) { return i; }

//------------------------------------------------------------------------------
// extent_type
//------------------------------------------------------------------------------
template <class T>
using extent_type = decltype(make_extent(std::declval<T>()));
}
}
