#pragma once

#include <echo/k_array/dimensionality.h>
#include <echo/k_array/shape_traits.h>
#include <echo/k_array/concept.h>
#include <echo/concept.h>
#include <echo/const_algorithm.h>
#include <echo/index.h>
#include <echo/assert.h>
#include <echo/concept2.h>

namespace echo {
namespace k_array {

///////////////////////
// is_dimensionality //
///////////////////////

TICK_TRAIT(is_dimensionality) {
  template <IndexInteger... Values>
  auto requires_(StaticIndex<Values...> x)
      ->tick::valid<
          is_true_c<const_algorithm::and_(fatal::constant_sequence<
              bool, (Values > 0) || (Values == Dimension::Dynamic())...>())>,
          is_true_c<(sizeof...(Values) > 0)>>;
};

////////////////////////
// is_stride_sequence //
////////////////////////

TICK_TRAIT(is_stride_sequence, is_dimensionality<_>){};

/////////////////
// is_subshape //
/////////////////

TICK_TRAIT(is_subshape) {
  template <class Shape>
  auto requires_(const Shape& shape)
      ->tick::valid<is_true<is_dimensionality<typename Shape::Dimensionality>>,
                    is_true<is_stride_sequence<typename Shape::StrideSequence>>,
                    decltype(shape.template dynamic_extent<0>()),
                    decltype(shape.template dynamic_stride<0>()),
                    is_true_c<Shape::Dimensionality::size ==
                              Shape::StrideSequence::size>>;
};

/////////////////////////
// is_contiguous_shape //
/////////////////////////

TICK_TRAIT(is_contiguous_shape) {
  template <class Shape>
  auto requires_(const Shape& shape)
      ->tick::valid<is_true<is_dimensionality<typename Shape::Dimensionality>>,
                    decltype(shape.template dynamic_extent<0>()),
                    is_false<is_subshape<Shape>>>;
};

//////////////
// is_shape //
//////////////

TICK_TRAIT(is_shape) {
  template <class Shape>
  auto requires_(const Shape& shape)
      ->tick::valid<is_true_c<is_contiguous_shape<Shape>::value ||
                              is_subshape<Shape>::value>>;
};

/////////////////////
// is_static_shape //
/////////////////////

TICK_TRAIT(is_static_shape, is_shape<_>) {
  template <class Shape>
  auto requires_(const Shape& shape)
      ->tick::valid<is_false_c<const_algorithm::contains(
          typename Shape::Dimensionality(), Dimension::Dynamic())>>;
};

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
constexpr IndexInteger get_num_dimensions() {
  return std::decay<Shape>::type::Dimensionality::size;
}

//////////////////////
// is_static_extent //
//////////////////////

template <int I, class Shape, CONCEPT_REQUIRES(concept::shape<Shape>())>
constexpr bool is_static_extent() {
  return get<I>(shape_traits::dimensionality<Shape>()) != Dimension::kDynamic;
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
IndexInteger get_extent(const Shape& shape) {
  using namespace const_algorithm;
  constexpr int dynamic_index = count(
      left<I>(shape_traits::dimensionality<Shape>()), Dimension::Dynamic());
  return shape.template dynamic_extent<dynamic_index>();
}

//////////////////////
// is_static_stride //
//////////////////////

namespace detail {
namespace shape {

enum class StaticStrideType { ContiguousShape, Subshape, Invalid };

template <int I, class Shape, StaticStrideType>
struct IsStaticStrideImpl {};

template <int I, class Shape>
struct IsStaticStrideImpl<I, Shape, StaticStrideType::ContiguousShape>
    : std::integral_constant<bool,
                             !const_algorithm::contains(
                                 const_algorithm::left<I>(
                                     shape_traits::dimensionality<Shape>()),
                                 Dimension::Dynamic())> {};

template <int I, class Shape>
struct IsStaticStrideImpl<I, Shape, StaticStrideType::Subshape>
    : std::integral_constant<bool,
                             get<I>(shape_traits::stride_sequence<Shape>()) !=
                                 Stride::kDynamic> {};
}  // namespace shape
}  // end namespace detail

template <int I, class Shape>
using is_static_stride = detail::shape::IsStaticStrideImpl < I,
      Shape, concept::contiguous_shape<Shape>()
                 ? detail::shape::StaticStrideType::ContiguousShape
                 : concept::subshape<Shape>()
                       ? detail::shape::StaticStrideType::Subshape
                       : detail::shape::StaticStrideType::Invalid > ;

///////////////////////
// get_static_stride //
///////////////////////

template <int I, class Shape, CONCEPT_REQUIRES(is_contiguous_shape<Shape>() &&
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
constexpr IndexInteger get_stride(const Shape& shape) {
  return get_extent<I - 1>(shape) * get_stride<I - 1>(shape);
}

template <int I, class Shape, CONCEPT_REQUIRES(concept::subshape<Shape>() &&
                                               is_static_stride<I, Shape>())>
constexpr auto get_stride(const Shape& shape)
    -> decltype(get<I>(shape_traits::stride_sequence<Shape>())) {
  return {};
}

template <int I, class Shape, CONCEPT_REQUIRES(concept::subshape<Shape>() &&
                                               !is_static_stride<I, Shape>())>
IndexInteger get_stride(const Shape& shape) {
  using namespace const_algorithm;
  constexpr int dynamic_index =
      count(left<I>(shape_traits::stride_sequence<Shape>()), Stride::Dynamic());
  return shape.template dynamic_stride<dynamic_index>();
}

//////////////////////
// get_num_elements //
//////////////////////

template <class Shape, CONCEPT_REQUIRES(is_static_shape<Shape>())>
constexpr auto get_num_elements(const Shape& shape) -> StaticIndex<
    const_algorithm::product(shape_traits::dimensionality<Shape>())> {
  return {};
}

namespace detail {
namespace shape {

template <int I, class Shape, CONCEPT_REQUIRES(I == 0)>
IndexInteger get_num_elements_impl(const Shape& shape) {
  return get_extent<I>(shape);
}

template <int I, class Shape, CONCEPT_REQUIRES(I != 0)>
IndexInteger get_num_elements_impl(const Shape& shape) {
  return get_extent<I>(shape) * get_num_elements_impl<I - 1>(shape);
}
}  // namespace shape
}  // end namespace detail

template <class Shape, CONCEPT_REQUIRES(concept::shape<Shape>() &&
                                        !is_static_shape<Shape>())>
IndexInteger get_num_elements(const Shape& shape) {
  return detail::shape::get_num_elements_impl<get_num_dimensions<Shape>() - 1>(
      shape);
}

//////////////////
// get_1d_index //
//////////////////

namespace detail {
namespace shape {

template <int I, class Shape,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>())>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i) {
  return i;
}

template <int I, class Shape, class... Indexes,
          CONCEPT_REQUIRES(concept::contiguous_shape<Shape>())>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i,
                               Indexes... i_rest) {
  return i + get_extent<I>(shape) * get_1d_index_impl<I + 1>(shape, i_rest...);
}

template <int I, class Shape, CONCEPT_REQUIRES(concept::subshape<Shape>())>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i) {
  return get_stride<I>(shape) * i;
}

template <int I, class Shape, class... Indexes,
          CONCEPT_REQUIRES(concept::subshape<Shape>())>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i,
                               Indexes... i_rest) {
  return get_stride<I>(shape) * i + get_1d_index_impl<I + 1>(shape, i_rest...);
}
}  // namespace shape
}  // end namespace detail

template <class Shape, class... Indexes,
          CONCEPT_REQUIRES(concept::shape<Shape>())>
Index<1> get_1d_index(const Shape& shape, Indexes... indexes) {
  static_assert(sizeof...(Indexes) == get_num_dimensions<Shape>(),
                "number of indexes must match the rank of the dimensionality");
  static_assert(
      const_algorithm::and_(fatal::constant_sequence<
          bool, std::is_convertible<Indexes, IndexInteger>::value...>()),
      "indexes must be convertible to IndexInteger");
  return Index<1>{detail::shape::get_1d_index_impl<0>(shape, indexes...)};
};

///////////////////////////////////
// are_shapes_statically_unequal //
///////////////////////////////////

namespace detail {
namespace shape {

template <class Index1, class Index2>
struct AreDimensionalitiesStaticallyUnequal : std::false_type {};

template <IndexInteger Extent1, IndexInteger Extent2>
struct AreDimensionalitiesStaticallyUnequal<StaticIndex<Extent1>,
                                            StaticIndex<Extent2>>
    : std::integral_constant<bool, Extent1 != Dimension::kDynamic &&
                                       Extent2 != Dimension::kDynamic &&
                                       Extent1 != Extent2>

      {};

template <IndexInteger ExtentFirst1, IndexInteger ExtentFirst2,
          IndexInteger... ExtentsRest1, IndexInteger... ExtentsRest2>
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
          CONCEPT_REQUIRES(get_num_dimensions<Shape1>() !=
                           get_num_dimensions<Shape2>())>
constexpr bool are_shapes_statically_unequal() {
  return true;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(get_num_dimensions<Shape1>() ==
                               get_num_dimensions<Shape2>() &&
                           AreDimensionalitiesStaticallyUnequal<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>())>
constexpr bool are_shapes_statically_unequal() {
  return true;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(get_num_dimensions<Shape1>() ==
                               get_num_dimensions<Shape2>() &&
                           !AreDimensionalitiesStaticallyUnequal<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>())>
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

template <IndexInteger Extent1, IndexInteger Extent2>
struct AreDimensionalitiesStaticallyEqual<StaticIndex<Extent1>,
                                          StaticIndex<Extent2>>
    : std::integral_constant<bool, Extent1 != Dimension::kDynamic &&
                                       Extent2 != Dimension::kDynamic &&
                                       Extent1 == Extent2>

      {};

template <IndexInteger ExtentFirst1, IndexInteger ExtentFirst2,
          IndexInteger... ExtentsRest1, IndexInteger... ExtentsRest2>
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
          CONCEPT_REQUIRES(get_num_dimensions<Shape1>() !=
                           get_num_dimensions<Shape2>())>
constexpr bool are_shapes_statically_equal() {
  return false;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(get_num_dimensions<Shape1>() ==
                               get_num_dimensions<Shape2>() &&
                           AreDimensionalitiesStaticallyEqual<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>())>
constexpr bool are_shapes_statically_equal() {
  return true;
}

template <class Shape1, class Shape2,
          CONCEPT_REQUIRES(get_num_dimensions<Shape1>() ==
                               get_num_dimensions<Shape2>() &&
                           !AreDimensionalitiesStaticallyEqual<
                               shape_traits::dimensionality<Shape1>,
                               shape_traits::dimensionality<Shape2>>())>
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
          CONCEPT_REQUIRES(I == Shape1::Dimensionality::size)>
constexpr bool are_shapes_equal(const Shape1&, const Shape2&) {
  return true;
}

template <int I, class Shape1, class Shape2,
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
          CONCEPT_REQUIRES(
              concept::shape<Shape1>() && concept::shape<Shape2>() &&
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
          CONCEPT_REQUIRES(
              concept::shape<Shape1>() && concept::shape<Shape2>() &&
              !(concept::static_shape<Shape1>() &&
                concept::static_shape<Shape2>()) &&
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
                     std::true_type>())>
constexpr std::false_type operator!=(const Shape1& lhs, const Shape2& rhs) {
  return {};
}

template <
    class Shape1, class Shape2,
    CONCEPT_REQUIRES(
        concept::shape<Shape1>() && concept::shape<Shape2>() &&
        std::is_same<decltype(std::declval<Shape1>() == std::declval<Shape2>()),
                     std::false_type>())>
constexpr std::true_type operator!=(const Shape1& lhs, const Shape2& rhs) {
  return {};
}

template <
    class Shape1, class Shape2,
    CONCEPT_REQUIRES(
        concept::shape<Shape1>() && concept::shape<Shape2>() &&
        std::is_same<decltype(std::declval<Shape1>() == std::declval<Shape2>()),
                     bool>())>
bool operator!=(const Shape1& lhs, const Shape2& rhs) {
  return !(lhs == rhs);
}
}  // namespace k_array
}  // namespace echo
