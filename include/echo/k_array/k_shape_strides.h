#pragma once

#include <echo/k_array/k_sequence.h>

namespace echo {
namespace k_array {

//////////////////
// k_stride_tag //
//////////////////

namespace detail {
namespace k_shape_strides {
struct k_stride_tag {};
}  // namespace k_subshape
}  // end namespace detail

///////////////////
// KShapeStrides //
///////////////////

template <index_t... Strides>
class KShapeStrides
    : KSequence<detail::k_shape_strides::k_stride_tag,
                const_algorithm::count(StaticIndex<Strides...>(),
                                       Stride::Dynamic())> {
  static constexpr IndexInteger kNumDynamicStrides =
      const_algorithm::count(StaticIndex<Strides...>(), Stride::Dynamic());
  using StrideContainer =
      KSequence<detail::k_shape_strides::k_stride_tag, kNumDynamicStrides>;

 public:
  using StrideSequence = StaticIndex<Strides...>;

  template <class... StrideExtents,
            CONCEPT_REQUIRES(const_algorithm::and_c<std::is_convertible<
                                 StrideExtents, index_t>::value...>() &&
                             sizeof...(StrideExtents) == kNumDynamicStrides)>
  explicit KShapeStrides(StrideExtents... stride_extents)
      : StrideContainer(stride_extents...) {}

  explicit KShapeStrides(const Index<kNumDynamicStrides>& stride_extents)
      : StrideContainer(stride_extents) {}

  template <int I>
  IndexInteger dynamic_stride() const {
    return StrideContainer::template extent<I>();
  }

  const auto& dynamic_strides() const { return StrideContainer::extents(); }
};
}
}
