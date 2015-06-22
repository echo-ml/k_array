#pragma once

#include <echo/k_array2/concept.h>
#include <echo/k_array2/extent.h>
#include <echo/htl.h>
#include <array>

namespace echo {
namespace k_array {

/////////////////
// dimension_t //
/////////////////

namespace dimension_t {
static constexpr index_t dynamic = -1;
}

////////////////////
// Dimensionality //
////////////////////

namespace detail {
namespace dimensionality {
struct extents_tag {};
}
}

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

  decltype(auto) extents() const { return htl::unpack(*this); }
};

/////////////////////////
// make_dimensionality //
/////////////////////////

namespace detail {
namespace dimensionality {

template <std::size_t... Indexes, class Extent>
auto make_dimensionality_impl(
    std::index_sequence<Indexes...>,
    const std::array<Extent, sizeof...(Indexes)>& extents) {
  return Dimensionality<repeat_type_c<Indexes, index_t>...>(
      std::get<Indexes>(extents)...);
}

template <std::size_t... Indexes, class... Extents>
auto make_dimensionality_impl(std::index_sequence<Indexes...>,
                              const htl::Tuple<Extents...>& extents) {
  return Dimensionality<decltype(make_extent(htl::get<Indexes>(extents)))...>(
      make_extent(htl::get<Indexes>(extents))...);
}
}
}

template <std::size_t N, class Extent,
          CONCEPT_REQUIRES(std::is_convertible<Extent, index_t>::value)>
auto make_dimensionality(const std::array<Extent, N>& extents) {
  return detail::dimensionality::make_dimensionality_impl(
      std::make_index_sequence<N>(), extents);
}

/////////////
// ExtentC //
/////////////

template <index_t I>
using ExtentC =
    std::conditional_t<I == dimension_t::dynamic, index_t, StaticIndex<I>>;

/////////////////////
// DimensionalityC //
/////////////////////

template <index_t... ExtentsC>
using DimensionalityC = Dimensionality<ExtentC<ExtentsC>...>;

/////////////////////////
// make_dimensionality //
/////////////////////////

template <class... Extents,
          CONCEPT_REQUIRES(and_c<concept::extent<Extents>()...>())>
auto make_dimensionality(Extents... extents) {
  return Dimensionality<decltype(make_extent(extents))...>(
      make_extent(extents)...);
}

template <class... Extents,
          CONCEPT_REQUIRES(and_c<concept::extent<Extents>()...>())>
auto make_dimensionality(const htl::Tuple<Extents...>& extents) {
  return detail::dimensionality::make_dimensionality_impl(
      std::index_sequence_for<Extents...>(), extents);
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

template <class... Extents>
auto get_num_elements(const Dimensionality<Extents...>& dimensionality) {
  return htl::left_fold([](auto x, auto y) { return x * y; }, 1_index,
                        dimensionality.extents());
}

////////////////
// operator== //
////////////////

template <class... LhsExtents, class... RhsExtents>
auto operator==(const Dimensionality<LhsExtents...>& lhs,
                const Dimensionality<RhsExtents...>& rhs) {
  return lhs.extents() == rhs.extents();
}

////////////////
// operator!= //
////////////////

template <class... LhsExtents, class... RhsExtents>
auto operator!=(const Dimensionality<LhsExtents...>& lhs,
                const Dimensionality<RhsExtents...>& rhs) {
  return lhs.extents() != rhs.extents();
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

/////////////////////////
// num_free_dimensions //
/////////////////////////

namespace detail {
namespace dimensionality {

template <class Dimensionality>
auto num_free_dimensions_impl(Dimensionality dimensionality) {
  auto is_free_dimension = [](auto x) {
    return htl::integral_constant<
        bool, !std::is_same<decltype(x), StaticIndex<1>>::value>();
  };
  return htl::count_if(is_free_dimension, dimensionality.extents());
}
}
}

template <class Dimensionality,
          CONCEPT_REQUIRES(k_array::concept::dimensionality<Dimensionality>())>
constexpr int num_free_dimensions() {
  using Result = decltype(detail::dimensionality::num_free_dimensions_impl(
      std::declval<Dimensionality>()));
  return Result::value;
}

////////////////////
// free_dimension //
////////////////////

namespace detail {
namespace dimensionality {

template <class Dimensionality>
auto free_dimension_impl(Dimensionality dimensionality) {
  auto is_free_dimension = [](auto x) {
    return htl::integral_constant<
        bool, !std::is_same<decltype(x), StaticIndex<1>>::value>();
  };
  return htl::find_if(is_free_dimension, dimensionality.extents());
}
}
}

template <class Dimensionality,
          CONCEPT_REQUIRES(num_free_dimensions<Dimensionality>() == 1)>
constexpr int free_dimension() {
  using Result = decltype(detail::dimensionality::free_dimension_impl(
      std::declval<Dimensionality>()));
  return Result::value;
}
}
}
