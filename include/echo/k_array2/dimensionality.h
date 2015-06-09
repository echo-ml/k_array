#pragma once

#include <echo/k_array2/concept.h>
#include <echo/htl.h>

namespace echo {
namespace k_array {

////////////////////
// Dimensionality //
////////////////////

namespace detail { namespace dimensionality {
struct extents_tag {};
}}

template <class... Extents>
class Dimensionality
    : htl::Pack<detail::dimensionality::extents_tag, htl::Tuple<Extents...>> {
  using Base =
      htl::Pack<detail::dimensionality::extents_tag, htl::Tuple<Extents...>>;

 public:
  static constexpr int num_dimensions = sizeof...(Extents);
  Dimensionality() : Base(htl::Tuple<Extents...>(Extents()...)) {}
  explicit Dimensionality(Extents... extents)
      : Base(htl::Tuple<Extents...>(extents...)) {}

  template <int I, CONCEPT_REQUIRES(I >= 0 && I < sizeof...(Extents))>
  auto extent() const {
    return htl::get<I>(htl::unpack(*this));
  }

  decltype(auto) extents() const {
    return htl::unpack(*this);
  }
};

/////////////////////////
// make_dimensionality //
/////////////////////////

template <class... Extents,
          CONCEPT_REQUIRES(and_c<concept::extent<Extents>()...>())>
auto make_dimensionality(Extents... extents) {
  return Dimensionality<Extents...>(extents...);
}

////////////////
// get_extent //
////////////////

template <int I, class... Extents,
          CONCEPT_REQUIRES(I >= 0 && I < sizeof...(Extents))>
auto get_extent(const Dimensionality<Extents...>& dimensionality) {
  return dimensionality.template extent<I>();
}

//////////////////////
// get_num_elements //
//////////////////////

template<class... Extents>
auto get_num_elements(const Dimensionality<Extents...>& dimensionality) {
  return htl::left_fold(
    [](auto x, auto y) { return x*y; },
    1_index,
    dimensionality.extents());
}

}

namespace dimensionality_traits {

////////////////////
// num_dimensions //
////////////////////

template <class Dimensionality>
constexpr auto num_dimensions() -> decltype(Dimensionality::num_dimensions) {
  return Dimensionality::num_dimensions;
}

/////////////////
// extent_type //
/////////////////

template <int I, class Dimensionality>
using extent_type =
    decltype(k_array::get_extent<I>(std::declval<Dimensionality>()));
}
}
