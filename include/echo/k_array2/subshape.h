#pragma once

#include <echo/k_array2/concept.h>

namespace echo {
namespace k_array {

//////////////
// Subshape //
//////////////

namespace detail {
namespace subshape {
struct strides_tag {};
}
}

template <class Dimensionality, class Strides>
class Subshape : public Dimensionality,
                 htl::Pack<detail::subshape::strides_tag, Strides> {
  using StridesBase = htl::Pack<detail::subshape::strides_tag, Strides>;

 public:
  Subshape(const Dimensionality& dimensionality, const Strides& strides)
      : Dimensionality(dimensionality), StridesBase(strides) {}

  const auto& strides() const {
    return htl::unpack<detail::subshape::strides_tag>(*this);
  }
};

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
}
}
