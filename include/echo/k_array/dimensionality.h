#pragma once

#include <echo/index.h>
#include <type_traits>
#include <utility>

namespace echo { namespace k_array {

///////////////
// Dimension //
///////////////

struct Dimension {
  static const IndexInteger kDynamic = -1;
  using Dynamic = StaticIndex<kDynamic>;
};

////////////
// Stride //
////////////

struct Stride {
  static const IndexInteger kDynamic = -1;
  using Dynamic = StaticIndex<kDynamic>;
};

///////////
// Slice //
///////////

struct Slice {
  static const IndexInteger kFull = -2;
  using Full = StaticIndex<kFull>;

  static const int kNull = -3;
  using Null = StaticIndex<kNull>;
};

namespace extent_traits {

/////////////////
// extent_type //
/////////////////

template <index_t Extent>
using extent_type = std::conditional_t<Extent == Dimension::kDynamic, index_t,
                                       StaticIndex<Extent>>;

/////////////////
// extent_enum //
/////////////////

namespace detail { namespace dimensionality {
template<index_t I>
constexpr index_t extent_enum_impl(StaticIndex<I>) {
  return I;
}

constexpr index_t extent_enum_impl(index_t) {
  return Dimension::kDynamic;
}

}}

template<class Extent>
constexpr auto extent_enum() -> 
  decltype(detail::dimensionality::extent_enum_impl(std::declval<Extent>()))
{
  return detail::dimensionality::extent_enum_impl(Extent());
}

}

}  // namespace k_array
}  // namespace echo
