#pragma once

#include <echo/k_array/shape.h>
#include <echo/k_array/k_shape.h>
#include <echo/k_array/k_subshape.h>

namespace echo {
namespace k_array {

///////////////////////////
// count_dynamic_strides //
///////////////////////////

namespace detail {
namespace slice {

template <int I, class Shape>
constexpr int count_dynamic_strides() {
  return 0;
}

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Null>())>
constexpr int count_dynamic_strides();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, Slice::Null>())>
constexpr int count_dynamic_strides() {
  return !is_static_stride<I, Shape>() +
         count_dynamic_strides<I + 1, Shape, ExtentsRest...>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, Slice::Null>())>
constexpr int count_dynamic_strides() {
  return count_dynamic_strides<I + 1, Shape, ExtentsRest...>();
}
}
}  // end namespace detail

////////////////////////////
// set_contiguous_strides //
////////////////////////////

namespace detail {
namespace slice {

template <int StrideIndex, int DynamicStrideIndex, class PrevStride,
          class Shape, int N>
void set_contiguous_strides(const Shape&, PrevStride,
                            Index<N>& dynamic_strides) {}

template <int StrideIndex, int DynamicStrideIndex, class PrevStride,
          class Shape, class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, Slice::Null>() &&
                           !is_static_stride<StrideIndex, Shape>())>
void set_contiguous_strides(const Shape& shape, PrevStride prev_stride,
                            Index<N>& dynamic_strides, ExtentFirst extent_first,
                            ExtentsRest... extents_rest);

template <int StrideIndex, int DynamicStrideIndex, class PrevStride,
          class Shape, class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Null>() ||
                           is_static_stride<StrideIndex, Shape>())>
void set_contiguous_strides(const Shape& shape, PrevStride prev_stride,
                            Index<N>& dynamic_strides, ExtentFirst extent_first,
                            ExtentsRest... extents_rest) {
  set_contiguous_strides<StrideIndex + 1, DynamicStrideIndex>(
      shape, prev_stride * get_extent<StrideIndex>(shape), dynamic_strides,
      extents_rest...);
}

template <
    int StrideIndex, int DynamicStrideIndex, class PrevStride, class Shape,
    class ExtentFirst, class... ExtentsRest, int N,
    CONCEPT_REQUIRES_REDECLARATION(!std::is_same<ExtentFirst, Slice::Null>() &&
                                   !is_static_stride<StrideIndex, Shape>())>
void set_contiguous_strides(const Shape& shape, PrevStride prev_stride,
                            Index<N>& dynamic_strides, ExtentFirst extent_first,
                            ExtentsRest... extents_rest) {
  dynamic_strides[DynamicStrideIndex] = prev_stride;
  set_contiguous_strides<StrideIndex + 1, DynamicStrideIndex + 1>(
      shape, prev_stride * get_extent<StrideIndex>(shape), dynamic_strides,
      extents_rest...);
}
}
}  // end namespace detail

//////////////////////////
// set_subshape_strides //
//////////////////////////

namespace detail {
namespace slice {

template <int StrideIndex, int DynamicStrideIndex, class Shape, int N>
void set_subshape_strides(const Shape&, Index<N>& dynamic_strides) {}

template <int StrideIndex, int DynamicStrideIndex, class Shape,
          class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, Slice::Null>() &&
                           !is_static_stride<StrideIndex, Shape>())>
void set_subshape_strides(const Shape& shape, Index<N>& dynamic_strides,
                          ExtentFirst extent_first,
                          ExtentsRest... extents_rest);

template <int StrideIndex, int DynamicStrideIndex, class Shape,
          class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Null>() ||
                           is_static_stride<StrideIndex, Shape>())>
void set_subshape_strides(const Shape& shape, Index<N>& dynamic_strides,
                          ExtentFirst extent_first,
                          ExtentsRest... extents_rest) {
  set_subshape_strides<StrideIndex + 1, DynamicStrideIndex>(
      shape, dynamic_strides, extents_rest...);
}

template <
    int StrideIndex, int DynamicStrideIndex, class Shape, class ExtentFirst,
    class... ExtentsRest, int N,
    CONCEPT_REQUIRES_REDECLARATION(!std::is_same<ExtentFirst, Slice::Null>() &&
                                   !is_static_stride<StrideIndex, Shape>())>
void set_subshape_strides(const Shape& shape, Index<N>& dynamic_strides,
                          ExtentFirst extent_first,
                          ExtentsRest... extents_rest) {
  dynamic_strides[DynamicStrideIndex] = get_stride<StrideIndex>(shape);
  set_subshape_strides<StrideIndex + 1, DynamicStrideIndex + 1>(
      shape, dynamic_strides, extents_rest...);
}
}
}  // end namespace detail

/////////////////
// set_strides //
/////////////////

namespace detail {
namespace slice {

template <class Shape, class... SliceExtents, int N,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>())>
void set_strides(const Shape& shape, Index<N>& dynamic_strides,
                 SliceExtents... slice_extents) {
  set_contiguous_strides<0, 0>(shape, StaticIndex<1>(), dynamic_strides,
                               slice_extents...);
}

template <class Shape, class... SliceExtents, int N,
          CONCEPT_REQUIRES(concept::subshape<Shape>())>
void set_strides(const Shape& shape, Index<N>& dynamic_strides,
                 SliceExtents... slice_extents) {
  set_subshape_strides<0, 0>(shape, dynamic_strides, slice_extents...);
}
}
}  // end namespace detail

/////////////////////////
// get_stride_sequence //
/////////////////////////

namespace detail {
namespace slice {

template <int I, class Shape>
auto get_stride_sequence() {
  return fatal::constant_sequence<IndexInteger>();
}

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(is_static_stride<I, Shape>() &&
                           !std::is_same<ExtentFirst, Slice::Null>())>
auto get_stride_sequence();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(!is_static_stride<I, Shape>() &&
                           !std::is_same<ExtentFirst, Slice::Null>())>
auto get_stride_sequence();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Null>())>
auto get_stride_sequence() {
  return get_stride_sequence<I + 1, Shape, ExtentsRest...>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(is_static_stride<I, Shape>() &&
                                   !std::is_same<ExtentFirst, Slice::Null>())>
auto get_stride_sequence() {
  using Tail = decltype(get_stride_sequence<I + 1, Shape, ExtentsRest...>());
  return typename Tail::template push_front<get_static_stride<I, Shape>()>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(!is_static_stride<I, Shape>() &&
                                   !std::is_same<ExtentFirst, Slice::Null>())>
auto get_stride_sequence() {
  using Tail = decltype(get_stride_sequence<I + 1, Shape, ExtentsRest...>());
  return typename Tail::template push_front<Stride::kDynamic>();
}
}
}  // end namespace detail

///////////////////////////
// count_dynamic_extents //
///////////////////////////

namespace detail {
namespace slice {

template <int I, class Shape>
constexpr int count_dynamic_extents() {
  return 0;
}

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Full>())>
constexpr int count_dynamic_extents();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, IndexInteger>() &&
                           (ExtentFirst::value > 0 ||
                            std::is_same<ExtentFirst, Slice::Null>()))>
constexpr int count_dynamic_extents();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, IndexInteger>())>
constexpr int count_dynamic_extents() {
  return 1 + count_dynamic_extents<I + 1, Shape, ExtentsRest...>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, Slice::Full>())>
constexpr int count_dynamic_extents() {
  return !is_static_extent<I, Shape>() +
         count_dynamic_extents<I + 1, Shape, ExtentsRest...>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(!std::is_same<ExtentFirst, IndexInteger>() &&
                                   (ExtentFirst::value > 0 ||
                                    std::is_same<ExtentFirst, Slice::Null>()))>
constexpr int count_dynamic_extents() {
  return count_dynamic_extents<I + 1, Shape, ExtentsRest...>();
}
}
}  // end namespace detail

/////////////////////////
// set_dynamic_extents //
/////////////////////////

namespace detail {
namespace slice {

template <int ExtentIndex, int DynamicExtentIndex, class Shape, int N>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents) {}

template <int ExtentIndex, int DynamicExtentIndex, class Shape,
          class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, IndexInteger>())>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first, ExtentsRest... extents_rest);

template <int ExtentIndex, int DynamicExtentIndex, class Shape,
          class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, IndexInteger>() &&
                           (ExtentFirst::value > 0 ||
                            std::is_same<ExtentFirst, Slice::Null>()))>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first, ExtentsRest... extents_rest);
template <int ExtentIndex, int DynamicExtentIndex, class Shape,
          class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Full>() &&
                           !is_static_extent<ExtentIndex, Shape>())>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first, ExtentsRest... extents_rest);

template <int ExtentIndex, int DynamicExtentIndex, class Shape,
          class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Full>() &&
                           is_static_extent<ExtentIndex, Shape>())>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first,
                         ExtentsRest... extents_rest) {
  set_dynamic_extents<ExtentIndex + 1, DynamicExtentIndex>(
      shape, dynamic_extents, extents_rest...);
}

template <
    int ExtentIndex, int DynamicExtentIndex, class Shape, class ExtentFirst,
    class... ExtentsRest, int N,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, Slice::Full>() &&
                                   !is_static_extent<ExtentIndex, Shape>())>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first,
                         ExtentsRest... extents_rest) {
  dynamic_extents[DynamicExtentIndex] = get_extent<ExtentIndex>(shape);
  set_dynamic_extents<ExtentIndex + 1, DynamicExtentIndex + 1>(
      shape, dynamic_extents, extents_rest...);
}

template <
    int ExtentIndex, int DynamicExtentIndex, class Shape, class ExtentFirst,
    class... ExtentsRest, int N,
    CONCEPT_REQUIRES_REDECLARATION(!std::is_same<ExtentFirst, IndexInteger>() &&
                                   (ExtentFirst::value > 0 ||
                                    std::is_same<ExtentFirst, Slice::Null>()))>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first,
                         ExtentsRest... extents_rest) {
  return set_dynamic_extents<ExtentIndex + 1, DynamicExtentIndex>(
      shape, dynamic_extents, extents_rest...);
}

template <
    int ExtentIndex, int DynamicExtentIndex, class Shape, class ExtentFirst,
    class... ExtentsRest, int N,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, IndexInteger>())>
void set_dynamic_extents(const Shape& shape, Index<N>& dynamic_extents,
                         ExtentFirst extent_first,
                         ExtentsRest... extents_rest) {
  dynamic_extents[DynamicExtentIndex] = extent_first;
  set_dynamic_extents<ExtentIndex + 1, DynamicExtentIndex + 1>(
      shape, dynamic_extents, extents_rest...);
}
}
}  // end namespace detail

/////////////////////
// reshape_extents //
/////////////////////

namespace detail {
namespace slice {

template <int I, class Shape>
auto reshape_extents() {
  return fatal::constant_sequence<IndexInteger>();
}

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Full>())>
auto reshape_extents();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, Slice::Null>())>
auto reshape_extents();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, IndexInteger>() &&
                           (ExtentFirst::value > 0))>
auto reshape_extents();

template <int I, class Shape, class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, IndexInteger>())>
auto reshape_extents() {
  using Tail = decltype(reshape_extents<I + 1, Shape, ExtentsRest...>());
  return typename Tail::template push_front<Dimension::kDynamic>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, Slice::Null>())>
auto reshape_extents() {
  return reshape_extents<I + 1, Shape, ExtentsRest...>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, Slice::Full>())>
auto reshape_extents() {
  using Tail = decltype(reshape_extents<I + 1, Shape, ExtentsRest...>());
  return typename Tail::template push_front<get<I>(Dimensionality<Shape>())>();
}

template <
    int I, class Shape, class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(!std::is_same<ExtentFirst, IndexInteger>() &&
                                   (ExtentFirst::value > 0))>
auto reshape_extents() {
  using Tail = decltype(reshape_extents<I + 1, Shape, ExtentsRest...>());
  return typename Tail::template push_front<ExtentFirst::value>();
}
}
}  // end namespace detail

/////////////
// reshape //
/////////////

namespace detail {
namespace slice {

template <IndexInteger... Dimensions, int N>
auto reshape_impl(const Index<N>& dynamic_extents,
                  fatal::constant_sequence<IndexInteger, Dimensions...>) {
  return KShape<Dimensions...>(dynamic_extents);
}
}
}  // end namespace detail

template <class Shape, class... SliceExtents,
          CONCEPT_REQUIRES(concept::shape<Shape>())>
auto reshape(const Shape& shape, SliceExtents... slice_extents) {
  constexpr int kNumDynamicExtents =
      detail::slice::count_dynamic_extents<0, Shape, SliceExtents...>();
  Index<kNumDynamicExtents> dynamic_extents;
  detail::slice::set_dynamic_extents<0, 0>(shape, dynamic_extents,
                                           slice_extents...);
  using Dimensionality =
      decltype(detail::slice::reshape_extents<0, Shape, SliceExtents...>());
  return detail::slice::reshape_impl(dynamic_extents, Dimensionality());
}

///////////
// slice //
///////////

namespace detail {
namespace slice {

template <class Shape, IndexInteger... Strides, int N>
auto slice_impl(const Shape& shape, const Index<N>& dynamic_strides,
                fatal::constant_sequence<IndexInteger, Strides...>) {
  return KSubshape<Shape, Strides...>(shape, dynamic_strides);
}
}
}  // end namesapce detail

template <class Shape, class... SliceExtents,
          CONCEPT_REQUIRES(concept::shape<Shape>())>
auto slice(const Shape& shape, SliceExtents... slice_extents) {
  static_assert(
      sizeof...(SliceExtents) == get_num_dimensions<Shape>(),
      "number of slice extents must match number of dimensions of shape");

  constexpr int kNumDynamicStrides =
      detail::slice::count_dynamic_strides<0, Shape, SliceExtents...>();
  Index<kNumDynamicStrides> dynamic_strides;
  detail::slice::set_strides(shape, dynamic_strides, slice_extents...);

  using StrideSequence =
      decltype(detail::slice::get_stride_sequence<0, Shape, SliceExtents...>());

  auto new_shape = reshape(shape, slice_extents...);

  return detail::slice::slice_impl(new_shape, dynamic_strides,
                                   StrideSequence());
}
}
}  // end namespace echo::k_array
