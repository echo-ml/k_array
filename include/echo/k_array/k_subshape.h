#pragma once

#include <echo/k_array/k_sequence.h>
#include <echo/k_array/shape.h>

namespace echo {
namespace k_array {

///////////////
// KSubshape //
///////////////

namespace detail {
namespace k_subshape {

struct k_stride_tag {};
}  // namespace k_subshape
}  // end namespace detail

template <class Shape, IndexInteger... Strides>
class KSubshape : public Shape,
                  KSequence<detail::k_subshape::k_stride_tag,
                            const_algorithm::count(StaticIndex<Strides...>(),
                                                   Stride::Dynamic())> {
  static constexpr IndexInteger kNumDynamicStrides =
      const_algorithm::count(StaticIndex<Strides...>(), Stride::Dynamic());
  using StrideContainer =
      KSequence<detail::k_subshape::k_stride_tag, kNumDynamicStrides>;

 public:
  using StrideSequence = StaticIndex<Strides...>;

  template <
      class... StrideExtents,
      CONCEPT_REQUIRES(const_algorithm::and_(fatal::constant_sequence<
          bool, std::is_convertible<StrideExtents, IndexInteger>::value...>()))>
  explicit KSubshape(const Shape& shape, StrideExtents... stride_extents)
      : Shape(shape), StrideContainer(stride_extents...) {}

  KSubshape(const Shape& shape, const Index<kNumDynamicStrides>& stride_extents)
      : Shape(shape), StrideContainer(stride_extents) {}

  template <int I>
  IndexInteger dynamic_stride() const {
    return StrideContainer::template extent<I>();
  }

  const auto& dynamic_strides() const { return StrideContainer::extents(); }
};
}  // namespace k_array
}  // namespace echo
