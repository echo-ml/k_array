#pragma once

#define DETAIL_NS detail_subshape

#include <echo/k_array/concept.h>
#include <echo/k_array/extent.h>
#include <echo/k_array/shape.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// Subshape
//------------------------------------------------------------------------------
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

  const auto& dimensionality() const {
    return static_cast<const Dimensionality&>(*this);
  }
};

//------------------------------------------------------------------------------
// make_strides
//------------------------------------------------------------------------------
template <class... Strides,
          CONCEPT_REQUIRES(and_c<concept::extent<Strides>()...>())>
auto make_strides(Strides... strides) {
  return htl::Tuple<decltype(make_extent(strides))...>(make_extent(strides)...);
}

//------------------------------------------------------------------------------
// is_contiguous_subshape
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <std::size_t... Indexes, class Dimensionality>
auto get_contiguous_strides_impl(std::index_sequence<Indexes...>,
                                 const Dimensionality& dimensionality) {
  auto shape = make_shape(dimensionality);
  return make_strides(get_stride<Indexes>(shape)...);
}

template <class... Extents>
auto get_contiguous_strides(const Dimensionality<Extents...>& dimensionality) {
  return get_contiguous_strides_impl(std::index_sequence_for<Extents...>(),
                                     dimensionality);
}

inline auto is_contiguous_subshape_impl(const htl::Tuple<>&,
                                        const htl::Tuple<>&,
                                        const htl::Tuple<>&) {
  return htl::integral_constant<bool, true>();
}

template <class DimensionFirst, class... DimensionsRest, class Stride1First,
          class... Strides1Rest, class Stride2First, class... Strides2Rest,
          CONCEPT_REQUIRES(!std::is_same<DimensionFirst, StaticIndex<1>>::value)>
auto is_contiguous_subshape_impl(
    const htl::Tuple<DimensionFirst, DimensionsRest...>& dimensions,
    const htl::Tuple<Stride1First, Strides1Rest...>& strides1,
    const htl::Tuple<Stride2First, Strides2Rest...>& strides2);

template <class DimensionFirst, class... DimensionsRest, class Stride1First,
          class... Strides1Rest, class Stride2First, class... Strides2Rest,
          CONCEPT_REQUIRES(std::is_same<DimensionFirst, StaticIndex<1>>::value)>
auto is_contiguous_subshape_impl(
    const htl::Tuple<DimensionFirst, DimensionsRest...>& dimensions,
    const htl::Tuple<Stride1First, Strides1Rest...>& strides1,
    const htl::Tuple<Stride2First, Strides2Rest...>& strides2) {
  return is_contiguous_subshape_impl(htl::tail(dimensions), htl::tail(strides1),
                                     htl::tail(strides2));
}

template <class DimensionFirst, class... DimensionsRest, class Stride1First,
          class... Strides1Rest, class Stride2First, class... Strides2Rest,
          CONCEPT_REQUIRES_REDECLARATION(
              !std::is_same<DimensionFirst, StaticIndex<1>>::value)>
auto is_contiguous_subshape_impl(
    const htl::Tuple<DimensionFirst, DimensionsRest...>& dimensions,
    const htl::Tuple<Stride1First, Strides1Rest...>& strides1,
    const htl::Tuple<Stride2First, Strides2Rest...>& strides2) {
  return htl::head(strides1) == htl::head(strides2) &&
         is_contiguous_subshape_impl(htl::tail(dimensions), htl::tail(strides1),
                                     htl::tail(strides2));
}
}

template <
    class Dimensionality, class Strides,
    CONCEPT_REQUIRES(concept::dimensionality<Dimensionality>() &&
                     concept::index_tuple<Strides>() &&
                     dimensionality_traits::num_dimensions<Dimensionality>() ==
                         htl::tuple_traits::num_elements<Strides>())>
auto is_contiguous_subshape(const Dimensionality& dimensionality,
                            const Strides& strides) {
  auto contiguous_strides = DETAIL_NS::get_contiguous_strides(dimensionality);
  return DETAIL_NS::is_contiguous_subshape_impl(dimensionality.extents(),
                                                strides, contiguous_strides);
}

//------------------------------------------------------------------------------
// make_subshape
//------------------------------------------------------------------------------
template <
    class Dimensionality, class Strides,
    CONCEPT_REQUIRES(
        concept::dimensionality<Dimensionality>() &&
        concept::index_tuple<Strides>() &&
        !htl::concept::boolean_true_constant<decltype(is_contiguous_subshape(
            std::declval<Dimensionality>(), std::declval<Strides>()))>())>
auto make_subshape(const Dimensionality& dimensionality,
                   const Strides& strides) {
  return Subshape<Dimensionality, Strides>(dimensionality, strides);
}

template <
    class Dimensionality, class Strides,
    CONCEPT_REQUIRES(
        concept::dimensionality<Dimensionality>() &&
        concept::index_tuple<Strides>() &&
        htl::concept::boolean_true_constant<decltype(is_contiguous_subshape(
            std::declval<Dimensionality>(), std::declval<Strides>()))>())>
auto make_subshape(const Dimensionality& dimensionality,
                   const Strides& strides) {
  return make_shape(dimensionality);
}

//------------------------------------------------------------------------------
// get_stride
//------------------------------------------------------------------------------
template <int I, class Dimensionality, class Strides>
auto get_stride(const Subshape<Dimensionality, Strides>& subshape) {
  return htl::get<I>(subshape.strides());
}

//------------------------------------------------------------------------------
// get_1d_index
//------------------------------------------------------------------------------
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
    class... Strides, class... Indexes,
    CONCEPT_REQUIRES(sizeof...(Strides) == sizeof...(Indexes) &&
                     and_c<concept::extent<Strides>()...>() &&
                     and_c<std::is_convertible<Indexes, index_t>::value...>())>
index_t get_1d_index(const htl::Tuple<Strides...>& strides,
                     Indexes... indexes) {
  return DETAIL_NS::get_1d_index_impl(strides, indexes...);
}

template <
    class... Extents, class Strides, class... Indexes,
    CONCEPT_REQUIRES(sizeof...(Extents) == sizeof...(Indexes) &&
                     and_c<std::is_convertible<Indexes, index_t>::value...>())>
index_t get_1d_index(
    const Subshape<Dimensionality<Extents...>, Strides>& subshape,
    Indexes... indexes) {
  CONTRACT_EXPECT {
    CONTRACT_ASSERT(within_dimensions(subshape, {indexes...}));
  };
  return get_1d_index(subshape.strides(), indexes...);
}
}
}

#undef DETAIL_NS
