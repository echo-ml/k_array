#pragma once

#include <echo/enable_if.h>
#include <echo/index.h>
#include <echo/const_algorithm.h>
#include <echo/k_array/shape.h>

namespace echo { namespace k_array {

////////////
// KShape //
////////////

namespace detail {

template<int N>
class KShapeBase {
 public:
  template<
      class... Extents
  >
  KShapeBase(Extents... extents) 
    : _extents(extents...)
  {
  }

  template<int I>
  IndexInteger extent() const {
    return get<I>(_extents);
  }
 private:
  Index<N> _extents;
};

template<>
class KShapeBase<0> {
};

} //end namespace detail

template<IndexInteger... Dimensions>
class KShape 
  : detail::KShapeBase<
        const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic())
  >
{
  static constexpr IndexInteger kNumDynamicExtents = 
              const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic());

  using Base = detail::KShapeBase<kNumDynamicExtents>;

 public:
  using Dimensionality = StaticIndex<Dimensions...>;

  template<
      class... Extents
    , enable_if<std::is_convertible<Extents, IndexInteger>...> = 0
  >
  KShape(Extents... dynamic_extents)
    : Base(dynamic_extents...)
  {}

  template<int I>
  IndexInteger dynamic_extent() const {
    return Base::template extent<I>();
  }
};

}} //end namespace
