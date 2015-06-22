#pragma once

#include <echo/index.h>

namespace echo {
namespace k_array {

template <index_t I>
auto make_extent(StaticIndex<I> i) {
  return i;
}

inline index_t make_extent(index_t i) { return i; }
}
}
