#pragma once

#include <echo/k_array/dimensionality.h>
#include <echo/k_array/shape_traits.h>
#include <echo/k_array/concept.h>
#include <echo/const_algorithm.h>
#include <echo/index.h>
#include <echo/assert.h>
#include <echo/concept2.h>

namespace echo {
namespace k_array {

////////////////////
// Dimensionality //
////////////////////

template <class Shape>
using Dimensionality = typename std::decay<Shape>::type::Dimensionality;

////////////////////
// StrideSequence //
////////////////////

template <class Shape>
using StrideSequence = typename std::decay<Shape>::type::StrideSequence;

////////////////////////
// get_num_dimensions //
////////////////////////

template <class Shape, CONCEPT_REQUIRES(concept::shape<Shape>())>
constexpr index_t get_num_dimensions() {
  return std::decay<Shape>::type::Dimensionality::size;
}

//////////////////////
// is_static_extent //
//////////////////////

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::shape<Shape>() &&
                           I < shape_traits::num_dimensions<Shape>())>
constexpr bool is_static_extent() {
  return shape_traits::static_extent<I, Shape>();
}

////////////////
// get_extent //
////////////////

template <int I, class Shape, CONCEPT_REQUIRES(concept::shape<Shape>() &&
                                               is_static_extent<I, Shape>())>
constexpr auto get_extent(const Shape& shape)
    -> decltype(get<I>(shape_traits::dimensionality<Shape>())) {
  return {};
}

template <int I, class Shape, CONCEPT_REQUIRES(concept::shape<Shape>() &&
                                               !is_static_extent<I, Shape>())>
index_t get_extent(const Shape& shape) {
  using namespace const_algorithm;
  constexpr int dynamic_index = count(
      left<I>(shape_traits::dimensionality<Shape>()), Dimension::Dynamic());
  return shape.template dynamic_extent<dynamic_index>();
}

//////////////////////
// is_static_stride //
//////////////////////

template <int I, class Shape, CONCEPT_REQUIRES(concept::shape<Shape>() ||
                                               concept::shape_strides<Shape>())>
constexpr bool is_static_stride() {
  return shape_traits::static_stride<I, Shape>();
}

///////////////////////
// get_static_stride //
///////////////////////

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>() &&
                           is_static_stride<I, Shape>())>
constexpr int get_static_stride() {
  return const_algorithm::product(
      const_algorithm::left<I>(shape_traits::dimensionality<Shape>()));
}

////////////////
// get_stride //
////////////////

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>() &&
                           is_static_stride<I, Shape>())>
constexpr auto get_stride(const Shape& shape)
    -> StaticIndex<get_static_stride<I, Shape>()> {
  return {};
}

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>() &&
                           !is_static_stride<I, Shape>())>
constexpr index_t get_stride(const Shape& shape) {
  return get_extent<I - 1>(shape) * get_stride<I - 1>(shape);
}

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::shape_strides<Shape>() &&
                           is_static_stride<I, Shape>())>
constexpr auto get_stride(const Shape& shape)
    -> decltype(get<I>(shape_traits::stride_sequence<Shape>())) {
  return {};
}

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::shape_strides<Shape>() &&
                           !is_static_stride<I, Shape>())>
index_t get_stride(const Shape& shape) {
  using namespace const_algorithm;
  constexpr int dynamic_index =
      count(left<I>(shape_traits::stride_sequence<Shape>()), Stride::Dynamic());
  return shape.template dynamic_stride<dynamic_index>();
}

//////////////////////
// get_num_elements //
//////////////////////

template <class Shape, CONCEPT_REQUIRES(concept::static_shape<Shape>())>
constexpr auto get_num_elements(const Shape& shape) -> StaticIndex<
    const_algorithm::product(shape_traits::dimensionality<Shape>())> {
  return {};
}

namespace detail {
namespace shape {

template <int I, class Shape, CONCEPT_REQUIRES(I == 0)>
index_t get_num_elements_impl(const Shape& shape) {
  return get_extent<I>(shape);
}

template <int I, class Shape, CONCEPT_REQUIRES(I != 0)>
index_t get_num_elements_impl(const Shape& shape) {
  return get_extent<I>(shape) * get_num_elements_impl<I - 1>(shape);
}
}  // namespace shape
}  // end namespace detail

template <class Shape, CONCEPT_REQUIRES(concept::shape<Shape>() &&
                                        !concept::static_shape<Shape>())>
index_t get_num_elements(const Shape& shape) {
  return detail::shape::get_num_elements_impl<
      shape_traits::num_dimensions<Shape>() - 1>(shape);
}

//////////////////
// get_1d_index //
//////////////////

namespace detail {
namespace shape {

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>())>
index_t get_1d_index_impl(const Shape& shape, index_t i) {
  return i;
}

template <int I, class Shape, class... Indexes,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>())>
index_t get_1d_index_impl(const Shape& shape, index_t i, Indexes... i_rest) {
  return i + get_extent<I>(shape) * get_1d_index_impl<I + 1>(shape, i_rest...);
}

template <int I, class Shape, CONCEPT_REQUIRES(concept::subshape<Shape>())>
index_t get_1d_index_impl(const Shape& shape, index_t i) {
  return get_stride<I>(shape) * i;
}

template <int I, class Shape, class... Indexes,
          CONCEPT_REQUIRES(concept::subshape<Shape>())>
index_t get_1d_index_impl(const Shape& shape, index_t i, Indexes... i_rest) {
  return get_stride<I>(shape) * i + get_1d_index_impl<I + 1>(shape, i_rest...);
}
}  // namespace shape
}  // end namespace detail

template <class Shape, class... Indexes,
          CONCEPT_REQUIRES(concept::shape<Shape>())>
Index<1> get_1d_index(const Shape& shape, Indexes... indexes) {
  static_assert(sizeof...(Indexes) == get_num_dimensions<Shape>(),
                "number of indexes must match the rank of the dimensionality");
  static_assert(const_algorithm::and_(fatal::constant_sequence<
                    bool, std::is_convertible<Indexes, index_t>::value...>()),
                "indexes must be convertible to index_t");
  return Index<1>{detail::shape::get_1d_index_impl<0>(shape, indexes...)};
};

///////////////////////////////////
// are_shapes_statically_unequal //
///////////////////////////////////

namespace detail {
namespace shape {

template <class Index1, class Index2>
struct AreDimensionalitiesStaticallyUnequal : std::false_type {};

template <index_t Extent1, index_t Extent2>
struct AreDimensionalitiesStaticallyUnequal<StaticIndex<Extent1>,
                                            StaticIndex<Extent2>>
    : std::integral_constant<bool, Extent1 != Dimension::kDynamic &&
                                       Extent2 != Dimension::kDynamic &&
                                       Extent1 != Extent2>

      {};

template <index_t ExtentFirst1, index_t ExtentFirst2, index_t... ExtentsRest1,
          index_t... ExtentsRest2>
struct AreDimensionalitiesStaticallyUnequal<
    StaticIndex<ExtentFirst1, ExtentsRest1...>,
    StaticIndex<ExtentFirst2, ExtentsRest2...>>
    : std::integral_constant<bool, (ExtentFirst1 != Dimension::kDynamic &&
                                    ExtentFirst2 != Dimension::kDynamic &&
                                    ExtentFirst1 != ExtentFirst2) ||
                                       AreDimensionalitiesStaticallyUnequal<
                                           StaticIndex<ExtentsRest1...>,
                                           StaticIndex<ExtentsRest2...>>::value

                             > {};

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(shape_traits::num_dimensions<Shape1>() !=
                           shape_traits::num_dimensions<Shape2>())>
constexpr bool are_shapes_statically_unequal() {
  return true;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(shape_traits::num_dimensions<Shape1>() ==
                               shape_traits::num_dimensions<Shape2>() &&
                           AreDimensionalitiesStaticallyUnequal<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>::value)>
constexpr bool are_shapes_statically_unequal() {
  return true;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(shape_traits::num_dimensions<Shape1>() ==
                               shape_traits::num_dimensions<Shape2>() &&
                           !AreDimensionalitiesStaticallyUnequal<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>::value)>
constexpr bool are_shapes_statically_unequal() {
  return false;
}
}  // namespace shape
}  // end namespace detail

/////////////////////////////////
// are_shapes_statically_equal //
/////////////////////////////////

namespace detail {
namespace shape {

template <class Index1, class Index2>
struct AreDimensionalitiesStaticallyEqual : std::true_type {};

template <index_t Extent1, index_t Extent2>
struct AreDimensionalitiesStaticallyEqual<StaticIndex<Extent1>,
                                          StaticIndex<Extent2>>
    : std::integral_constant<bool, Extent1 != Dimension::kDynamic &&
                                       Extent2 != Dimension::kDynamic &&
                                       Extent1 == Extent2>

      {};

template <index_t ExtentFirst1, index_t ExtentFirst2, index_t... ExtentsRest1,
          index_t... ExtentsRest2>
struct AreDimensionalitiesStaticallyEqual<
    StaticIndex<ExtentFirst1, ExtentsRest1...>,
    StaticIndex<ExtentFirst2, ExtentsRest2...>>
    : std::integral_constant<bool,
                             (ExtentFirst1 != Dimension::kDynamic &&
                              ExtentFirst2 != Dimension::kDynamic &&
                              ExtentFirst1 == ExtentFirst2) &&
                                 AreDimensionalitiesStaticallyEqual<
                                     StaticIndex<ExtentsRest1...>,
                                     StaticIndex<ExtentsRest2...>>::value> {};

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(shape_traits::num_dimensions<Shape1>() !=
                           shape_traits::num_dimensions<Shape2>())>
constexpr bool are_shapes_statically_equal() {
  return false;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(shape_traits::num_dimensions<Shape1>() ==
                               shape_traits::num_dimensions<Shape2>() &&
                           AreDimensionalitiesStaticallyEqual<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>::value)>
constexpr bool are_shapes_statically_equal() {
  return true;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(shape_traits::num_dimensions<Shape1>() ==
                               shape_traits::num_dimensions<Shape2>() &&
                           !AreDimensionalitiesStaticallyEqual<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>::value)>
constexpr bool are_shapes_statically_equal() {
  return false;
}
}  // namespace shape
}  // end namespace detail

//////////////////////
// are_shapes_equal //
//////////////////////

namespace detail {
namespace shape {

template <int I, class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(I == Shape1::Dimensionality::size)>
constexpr bool are_shapes_equal(const Shape1&, const Shape2&) {
  return true;
}

template <int I, class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(I != Shape1::Dimensionality::size)>
bool are_shapes_equal(const Shape1& shape1, const Shape2& shape2) {
  return get_extent<I>(shape1) == get_extent<I>(shape2) &&
         are_shapes_equal<I + 1>(shape1, shape2);
}
}  // namespace shape
}  // end namespace detail

////////////////
// operator== //
////////////////

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>()),
          CONCEPT_REQUIRES(
              detail::shape::are_shapes_statically_unequal<Shape1, Shape2>())>
constexpr std::false_type operator==(const Shape1&, const Shape2&) {
  return {};
}
template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(
              concept::static_shape<Shape1>() &&
              concept::static_shape<Shape2>() &&
              detail::shape::are_shapes_statically_equal<Shape1, Shape2>())>
constexpr std::true_type operator==(const Shape1, const Shape2&) {
  return {};
}
template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(concept::shape<Shape1>() &&
                           concept::shape<Shape2>() &&
                           !(concept::static_shape<Shape1>() &&
                             concept::static_shape<Shape2>())),
          CONCEPT_REQUIRES(
              !detail::shape::are_shapes_statically_unequal<Shape1, Shape2>())>
bool operator==(const Shape1& shape1, const Shape2& shape2) {
  return detail::shape::are_shapes_equal<0>(shape1, shape2);
}

////////////////
// operator!= //
////////////////

template <
    class Shape1, class Shape2,
    CONCEPT_REQUIRES(
        concept::shape<Shape1>() && concept::shape<Shape2>() &&
        std::is_same<decltype(std::declval<Shape1>() == std::declval<Shape2>()),
                     std::true_type>::value)>
constexpr std::false_type operator!=(const Shape1& lhs, const Shape2& rhs) {
  return {};
}

template <
    class Shape1, class Shape2,
    CONCEPT_REQUIRES(
        concept::shape<Shape1>() && concept::shape<Shape2>() &&
        std::is_same<decltype(std::declval<Shape1>() == std::declval<Shape2>()),
                     std::false_type>::value)>
constexpr std::true_type operator!=(const Shape1& lhs, const Shape2& rhs) {
  return {};
}

template <
    class Shape1, class Shape2,
    CONCEPT_REQUIRES(
        concept::shape<Shape1>() && concept::shape<Shape2>() &&
        std::is_same<decltype(std::declval<Shape1>() == std::declval<Shape2>()),
                     bool>::value)>
bool operator!=(const Shape1& lhs, const Shape2& rhs) {
  return !(lhs == rhs);
}
}  // namespace k_array

namespace shape_traits {

/////////////////
// extent_type //
/////////////////

template <int I, class Shape>
using extent_type = decltype(k_array::get_extent<I>(std::declval<Shape>()));

/////////////////////////
// num_free_dimensions //
/////////////////////////

namespace detail {
namespace shape {

template <int I, class Shape, CONCEPT_REQUIRES(num_dimensions<Shape>() == I)>
constexpr int count_free_dimensions() {
  return 0;
}

template <int I, class Shape, CONCEPT_REQUIRES(num_dimensions<Shape>() != I)>
constexpr int count_free_dimensions() {
  return (std::is_same<extent_type<I, Shape>, StaticIndex<1>>::value ? 0 : 1) +
         count_free_dimensions<I + 1, Shape>();
}
}
}

template <class Shape>
constexpr int num_free_dimensions() {
  return detail::shape::count_free_dimensions<0, Shape>();
}
}

}  // namespace echo
