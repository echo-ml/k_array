#pragma once

#include <echo/enable_if.h>
#include <echo/index.h>
#include <echo/const_algorithm.h>
#include <echo/k_array/shape.h>

namespace echo { namespace k_array {

////////////
// KShape //
////////////

template<IndexInteger... Dimensions>
class KShape 
  : Index<
        const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic())
    >
{
  static constexpr IndexInteger kNumDynamicExtents = 
              const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic());

  using DynamicExtents = Index<kNumDynamicExtents>;

 public:
  using Dimensionality = StaticIndex<Dimensions...>;

  template<
      class... Extents
    , enable_if<std::is_convertible<Extents, IndexInteger>...> = 0
  >
  KShape(Extents... dynamic_extents)
    : DynamicExtents(dynamic_extents...)
  {}

  template<int I>
  IndexInteger dynamic_extent() const {
    return get<I>(*static_cast<const DynamicExtents*>(this));
  }
};

}} //end namespace
