#pragma once

#include <echo/k_array2/dimensionality.h>
#include <echo/k_array2/concept.h>
#include <echo/htl.h>

namespace echo {
namespace k_array {

///////////
// Shape //
///////////

template <class... Extents>
class Shape : public Dimensionality<Extents...> {
  using Base = Dimensionality<Extents...>;

 public:
  Shape() = default;
  Shape(const Base& dimensionality) : Base(dimensionality) {}

  Shape(Extents... extents) : Base(extents...) {}

  const auto& dimensionality() const { return static_cast<const Base&>(*this); }
};

////////////////
// make_shape //
////////////////

template <class... Extents>
auto make_shape(const Dimensionality<Extents...>& dimensionality) {
  return Shape<Extents...>(dimensionality);
}

template <class... Extents,
          CONCEPT_REQUIRES(and_c<concept::extent<Extents>()...>())>
auto make_shape(Extents... extents) {
  return Shape<Extents...>(extents...);
}

////////////////
// get_stride //
////////////////

template <int I, class... Extents,
          CONCEPT_REQUIRES(I >= 0 && I < sizeof...(Extents))>
auto get_stride(const Shape<Extents...>& shape) {
  using Extents_ = uncvref_t<decltype(
      htl::left<I + 1>(htl::Tuple<StaticIndex<1>, Extents...>()))>;
  decltype(auto) extents_ = reinterpret_cast<const Extents_&>(shape.extents());
  return htl::left_fold([](auto x, auto y) { return x * y; }, 1_index,
                        extents_);
}
}
}
