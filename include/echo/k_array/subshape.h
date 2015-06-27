#pragma once

#define DETAIL_NS detail_subshape

#include <echo/k_array/concept.h>
#include <echo/k_array/extent.h>

namespace echo {
namespace k_array {

//////////////
// Subshape //
//////////////

namespace DETAIL_NS {
struct strides_tag {};
}

template <class Dimensionality, class Strides>
class Subshape : public Dimensionality,
                 htl::Pack<DETAIL_NS::strides_tag, Strides> {
  using StridesBase = htl::Pack<DETAIL_NS::strides_tag, Strides>;

 public:
  Subshape() {}
  Subshape(const Dimensionality& dimensionality, const Strides& strides)
      : Dimensionality(dimensionality), StridesBase(strides) {}

  const auto& strides() const {
    return htl::unpack<DETAIL_NS::strides_tag>(*this);
  }
};

//////////////////
// make_strides //
//////////////////

template <class... Strides,
          CONCEPT_REQUIRES(and_c<concept::extent<Strides>()...>())>
auto make_strides(Strides... strides) {
  return htl::Tuple<decltype(make_extent(strides))...>(make_extent(strides)...);
}

///////////////////
// make_subshape //
///////////////////

template <class Dimensionality, class Strides,
          CONCEPT_REQUIRES(concept::dimensionality<Dimensionality>() &&
                           concept::index_tuple<Strides>())>
auto make_subshape(const Dimensionality& dimensionality,
                   const Strides& strides) {
  return Subshape<Dimensionality, Strides>(dimensionality, strides);
}

////////////////
// get_stride //
////////////////

template <int I, class Dimensionality, class Strides>
auto get_stride(const Subshape<Dimensionality, Strides>& subshape) {
  return htl::get<I>(subshape.strides());
}

//////////////////
// get_1d_index //
//////////////////

namespace DETAIL_NS {
template <class Stride>
index_t get_1d_index_impl(const htl::Tuple<Stride>& strides, index_t index) {
  return htl::head(strides) * index;
}

template <class StrideFirst, class... StridesRest, class... IndexesRest>
index_t get_1d_index_impl(
    const htl::Tuple<StrideFirst, StridesRest...>& strides, index_t index_first,
    IndexesRest... indexes_rest) {
  return htl::head(strides) * index_first +
         get_1d_index_impl(htl::tail(strides), indexes_rest...);
}
}

template <
    class... Extents, class Strides, class... Indexes,
    CONCEPT_REQUIRES(sizeof...(Extents) == sizeof...(Indexes) &&
                     and_c<std::is_convertible<Indexes, index_t>::value...>())>
index_t get_1d_index(
    const Subshape<Dimensionality<Extents...>, Strides>& subshape,
    Indexes... indexes) {
  return DETAIL_NS::get_1d_index_impl(subshape.strides(), indexes...);
}
}
}

#undef DETAIL_NS
