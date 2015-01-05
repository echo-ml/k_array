#pragma once

#include <echo/k_array/k_sequence.h>
#include <echo/k_array/shape.h>

namespace echo { namespace k_array {

///////////////
// KSubshape //
///////////////

namespace detail {

struct k_stride_tag {};

} //end namespace detail

template<class Shape, IndexInteger... Strides>
class KSubshape
  : public Shape
  , KSequence<
        detail::k_stride_tag
      , const_algorithm::count(StaticIndex<Strides...>(), Stride::Dynamic())
    >
{
  static constexpr IndexInteger kNumDynamicStrides = 
              const_algorithm::count(StaticIndex<Strides...>(), Stride::Dynamic());
  using StrideContainer = KSequence<detail::k_stride_tag, kNumDynamicStrides>;
 public:
  using StrideSequence = StaticIndex<Strides...>;

  template<
      class... StrideExtents
    , enable_if<std::is_convertible<StrideExtents, IndexInteger>...> = 0
  >
  KSubshape(const Shape& shape, StrideExtents... stride_extents)
    : Shape(shape)
    , StrideContainer(stride_extents...)
  {}

  KSubshape(const Shape& shape, const Index<kNumDynamicStrides>& stride_extents)
    : Shape(shape)
    , StrideContainer(stride_extents)
  {}

  template<int I>
  IndexInteger dynamic_stride() const {
    return StrideContainer::template extent<I>();
  }
};

}} //end namespace echo::k_array
