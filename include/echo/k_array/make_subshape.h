#pragma once

#define DETAIL_NS detail_make_subshape

#include <echo/k_array/slice.h>
#include <echo/k_array/shape_traits.h>
#include <echo/contract.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// get_subdimensions
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct empty_dimension {};

template <class Extent, CONCEPT_REQUIRES(concept::static_extent<Extent>())>
auto get_subdimension(Extent extent, slice::all_t) {
  return extent;
}

template <class Extent, CONCEPT_REQUIRES(concept::dynamic_extent<Extent>())>
index_t get_subdimension(Extent extent, slice::all_t) {
  return extent;
}

template <class Extent>
index_t get_subdimension(Extent, const k_array::slice::Range& range) {
  return range.b() - range.a();
}

template <class Extent>
auto get_subdimension(Extent, index_t) {
  return empty_dimension();
}

template <class Extent1, class Extent2,
          CONCEPT_REQUIRES(concept::static_extent<Extent2>())>
auto get_subdimension(
    Extent1, const k_array::slice::CountedRange<Extent2>& counted_range) {
  return counted_range.extent();
}

template <class Extent1, class Extent2,
          CONCEPT_REQUIRES(concept::dynamic_extent<Extent2>())>
index_t get_subdimension(
    Extent1, const k_array::slice::CountedRange<Extent2>& counted_range) {
  return counted_range.extent();
}

template <class Shape, class Slices>
auto get_subdimensions(const Shape& shape, const Slices& make_subshapes) {
  auto subdimensions = htl::map([](auto extent, auto make_subshape) {
    return k_array::DETAIL_NS::get_subdimension(extent, make_subshape);
  }, shape.extents(), make_subshapes);
  return make_dimensionality(htl::remove_if([](auto dimension) {
    return htl::integral_constant<
        bool, std::is_same<decltype(dimension),
                           k_array::DETAIL_NS::empty_dimension>::value>();
  }, subdimensions));
}
}

//------------------------------------------------------------------------------
// SubdimensionIndexes
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class... Slices>
auto get_subdimension_indexes_impl(const htl::Tuple<Slices...>& slices) {
  auto indexes = htl::make_index_sequence<sizeof...(Slices)>();
  auto is_null_slice = [&](auto index) {
    return htl::integral_constant<
        bool, k_array::concept::dynamic_extent<htl::tuple_traits::element_type<
                  decltype(index)::value, uncvref_t<decltype(slices)>>>()>();
  };
  return htl::remove_if(is_null_slice, indexes);
}

template <class Slices>
using SubdimensionIndexes =
    decltype(get_subdimension_indexes_impl(std::declval<Slices>()));
}

//------------------------------------------------------------------------------
// get_strides
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <std::size_t I, std::size_t J, class StrideI, class Shape>
auto get_stride(StrideI stride_i, const Shape& shape) {
  decltype(auto) extents_make_subshape = htl::slice<I, J>(shape.extents());
  return make_extent(
      htl::left_fold(functional::multiplies, stride_i, extents_make_subshape));
}

template <std::size_t I, class StrideI, class Functor, class Shape>
auto get_strides_impl(htl::index_sequence<I>, StrideI, const Functor& functor,
                      const Shape&) {
  return functor();
}

template <std::size_t I, std::size_t J, std::size_t... IndexesRest,
          class StrideI, class Functor, class Shape>
auto get_strides_impl(htl::integer_sequence<std::size_t, I, J, IndexesRest...>,
                      StrideI stride_i, const Functor& functor,
                      const Shape& shape) {
  auto stride_j = get_stride<I, J>(stride_i, shape);
  auto continuation =
      [&](auto... strides_rest) { return functor(stride_j, strides_rest...); };
  return get_strides_impl(htl::index_sequence<J, IndexesRest...>(), stride_j,
                          continuation, shape);
}

template <std::size_t... Indexes, class Shape>
auto get_strides_impl(htl::integer_sequence<std::size_t, Indexes...>,
                      const Shape& shape) {
  auto functor = [](auto... strides) { return htl::make_tuple(strides...); };
  return get_strides_impl(htl::index_sequence<0, Indexes...>(), 1_index,
                          functor, shape);
}

template <class Shape, class Slices>
auto get_strides(const Shape& shape, const Slices& slices) {
  return get_strides_impl(SubdimensionIndexes<Slices>(), shape);
}
}

//------------------------------------------------------------------------------
// is_subslice
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Extent>
bool is_subslice(Extent extent, slice::all_t) {
  return true;
}

template <class Extent>
bool is_subslice(Extent extent, const slice::Range& range) {
  return 0 <= range.a() && range.a() <= range.b() && range.b() <= extent;
}

template <class Extent1, class Extent2>
bool is_subslice(Extent1 extent,
                 const slice::CountedRange<Extent2>& counted_range) {
  return is_subslice(extent,
                     slice::Range(counted_range.a(),
                                  counted_range.a() + counted_range.extent()));
}

template <class Extent>
bool is_subslice(Extent extent, index_t i) {
  return 0 <= i && i < extent;
}

template <std::size_t... Indexes, class Shape, class Slices>
bool is_subshape_impl(std::index_sequence<Indexes...>, const Shape& shape,
                      const Slices& slices) {
  return and_(
      is_subslice(get_extent<Indexes>(shape), std::get<Indexes>(slices))...);
}
}

template <class Shape, class... Slices,
          CONCEPT_REQUIRES(
              concept::shape<Shape>() && and_c<concept::slice<Slices>()...>() &&
              shape_traits::num_dimensions<Shape>() == sizeof...(Slices))>
bool is_subshape(const Shape& shape, const Slices&... slices) {
  return DETAIL_NS::is_subshape_impl(std::index_sequence_for<Slices...>(),
                                     shape, std::make_tuple(slices...));
}

//------------------------------------------------------------------------------
// make_subshape
//------------------------------------------------------------------------------
template <class Shape, class... Slices,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>() &&
                           and_c<concept::slice<Slices>()...>() &&
                           shape_traits::num_dimensions<Shape>() ==
                               sizeof...(Slices) &&
                           !concept::proper_slices<Slices...>())>
auto make_subshape(const Shape& shape, const Slices&... slices) {
  CONTRACT_EXPECT { CONTRACT_ASSERT(is_subshape(shape, slices...)); };
  auto slices_tuple = htl::make_tuple(slices...);
  return make_subshape(DETAIL_NS::get_subdimensions(shape, slices_tuple),
                       DETAIL_NS::get_strides(shape, slices_tuple));
}

template <class Shape, class... Slices,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>() &&
                           and_c<concept::slice<Slices>()...>() &&
                           shape_traits::num_dimensions<Shape>() ==
                               sizeof...(Slices) &&
                           concept::proper_slices<Slices...>())>
auto make_subshape(const Shape& shape, const Slices&... slices) {
  CONTRACT_EXPECT { CONTRACT_ASSERT(is_subshape(shape, slices...)); };
  using SubdimensionIndexes =
      DETAIL_NS::SubdimensionIndexes<htl::Tuple<Slices...>>;
  return make_shape(
      htl::left<htl::tuple_traits::num_elements<SubdimensionIndexes>()>(
          shape.extents()));
}

template <class Shape, class... Slices,
          CONCEPT_REQUIRES(concept::subshape<Shape>() &&
                           and_c<concept::slice<Slices>()...>() &&
                           shape_traits::num_dimensions<Shape>() ==
                               sizeof...(Slices))>
auto make_subshape(const Shape& shape, const Slices&... slices) {
  CONTRACT_EXPECT { CONTRACT_ASSERT(is_subshape(shape, slices...)); };
  using SubdimensionIndexes =
      DETAIL_NS::SubdimensionIndexes<htl::Tuple<Slices...>>;
  auto slices_tuple = htl::make_tuple(slices...);
  return make_subshape(
      DETAIL_NS::get_subdimensions(shape, slices_tuple),
      htl::make_subtuple(SubdimensionIndexes(), shape.strides()));
}
}
}

#undef DETAIL_NS
