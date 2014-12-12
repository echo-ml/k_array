#pragma once

#include <echo/concept.h>
#include <echo/enable_if.h>
#include <echo/const_algorithm.h>
#include <echo/index.h>

namespace echo { namespace k_array {

///////////////
// Dimension //
///////////////

struct Dimension {
  static const IndexInteger kDynamic = -1;
  using Dynamic = StaticIndex<kDynamic>;
};

////////////
// Stride //
////////////

struct Stride {
  static const IndexInteger kDynamic = -1;
  using Dynamic = StaticIndex<kDynamic>;
};

///////////////////////
// is_dimensionality //
///////////////////////

TICK_TRAIT(is_dimensionality) {
  template<IndexInteger... Values>
  auto requires_(StaticIndex<Values...> x) -> tick::valid<
     is_true_c<
        const_algorithm::and_(
          fatal::constant_sequence<
              bool
            , (Values > 0) || (Values == Dimension::Dynamic())...
          >()
        )
     >
   , is_true_c<(sizeof...(Values) > 0)>
  >;
};

////////////////////////
// is_stride_sequence //
////////////////////////

TICK_TRAIT(
    is_stride_sequence
  , is_dimensionality<_>
) 
{
};

/////////////////
// is_subshape //
/////////////////

TICK_TRAIT(is_subshape) {
  template<class Shape>
  auto requires_(const Shape& shape) -> tick::valid<
      is_true<is_dimensionality<typename Shape::Dimensionality>>
    , is_true<is_stride_sequence<typename Shape::Strides>>
    , decltype(shape.template dynamic_extent<0>())
    , decltype(shape.template dynamic_stride<0>())
    , is_true_c<Shape::Dimensionality::size == Shape::Strides::size>
  >;
};

/////////////////////////
// is_contiguous_shape //
/////////////////////////

TICK_TRAIT(is_contiguous_shape) {
  template<class Shape>
  auto requires_(const Shape& shape) -> tick::valid<
      is_true<is_dimensionality<typename Shape::Dimensionality>>
    , decltype(shape.template dynamic_extent<0>())
    , is_false<is_subshape<Shape>>
  >;
};

//////////////
// is_shape //
//////////////

TICK_TRAIT(is_shape) {
  template<class Shape>
  auto requires_(const Shape& shape) -> tick::valid<
      is_true_c<is_contiguous_shape<Shape>::value || is_subshape<Shape>::value>
  >;
};

/////////////////////
// is_static_shape //
/////////////////////

TICK_TRAIT(
    is_static_shape
  , is_shape<_>
) 
{
  template<class Shape>
  auto requires_(const Shape& shape) -> tick::valid<
      is_false_c<
          const_algorithm::contains(
              typename Shape::Dimensionality()
            , Dimension::Dynamic()
          )
      >
  >;
};

////////////////////
// Dimensionality //
////////////////////

template<class Shape>
using Dimensionality = typename Shape::Dimensionality;

////////////////////
// StrideSequence //
////////////////////

template<class Shape>
using StrideSequence = typename Shape::Strides;

//////////////
// get_rank //
//////////////

template<
    class Shape
  , enable_if<is_shape<Shape>> = 0
>
constexpr IndexInteger get_rank() {
  return Shape::Dimensionality::size;
}

//////////////////////
// is_static_extent //
//////////////////////

template<
    int I
  , class Shape
  , enable_if<is_shape<Shape>> = 0
>
constexpr bool is_static_extent() {
  return get<I>(Dimensionality<Shape>()) != Dimension::kDynamic;
}

////////////////
// get_extent //
////////////////

template<
    int I
  , class Shape
  , enable_if<is_shape<Shape>> = 0
  , enable_if_c<is_static_extent<I, Shape>()> = 0
>
constexpr auto 
get_extent(const Shape& shape) ->
  decltype(get<I>(Dimensionality<Shape>()))  
{
  return {};
}

template<
    int I
  , class Shape
  , enable_if<is_shape<Shape>> = 0
  , disable_if_c<is_static_extent<I, Shape>()> = 0
>
IndexInteger get_extent(const Shape& shape) {
  using namespace const_algorithm;
  constexpr int dynamic_index = 
      count(left<I>(Dimensionality<Shape>()), Dimension::Dynamic());
  return shape.template dynamic_extent<dynamic_index>();
}

//////////////////////
// is_static_stride //
//////////////////////

namespace detail {

enum class StaticStrideType {
    ContiguousShape
  , Subshape
  , Invalid
};

template<int I, class Shape, StaticStrideType>
struct IsStaticStrideImpl {};

template<int I, class Shape>
struct IsStaticStrideImpl<
    I
  , Shape
  , StaticStrideType::ContiguousShape
> 
  : std::integral_constant<
        bool
      , !const_algorithm::contains(
          const_algorithm::left<I>(Dimensionality<Shape>()), Dimension::Dynamic()
        )
    >
{
};

template<int I, class Shape>
struct IsStaticStrideImpl<
    I
  , Shape
  , StaticStrideType::Subshape
>
  : std::integral_constant<
        bool
      , get<I>(StrideSequence<Shape>()) != Stride::kDynamic
    >
{
};

} //end namespace detail

template<int I, class Shape>
using is_static_stride =
    detail::IsStaticStrideImpl<
        I
      , Shape
      , is_contiguous_shape<Shape>::value
      ? detail::StaticStrideType::ContiguousShape
      : is_subshape<Shape>::value
      ? detail::StaticStrideType::Subshape 
      : detail::StaticStrideType::Invalid
    >;

////////////////
// get_stride //
////////////////

template<
    int I
  , class Shape
  , enable_if<is_contiguous_shape<Shape>> = 0
  , enable_if<is_static_stride<I, Shape>> = 0
>
constexpr auto get_stride(const Shape& shape) ->
    StaticIndex<
        const_algorithm::product(
            const_algorithm::left<I>(Dimensionality<Shape>())
        )
    >
{
  return {};
}

template<
    int I
  , class Shape
  , enable_if<is_contiguous_shape<Shape>> = 0
  , disable_if<is_static_stride<I, Shape>> = 0
>
constexpr IndexInteger get_stride(const Shape& shape) {
  return get_extent<I-1>(shape) * get_stride<I-1>(shape);
}

template<
    int I
  , class Shape
  , enable_if<is_subshape<Shape>> = 0
  , enable_if<is_static_stride<I, Shape>> = 0
>
constexpr auto get_stride(const Shape& shape) ->
    decltype(get<I>(StrideSequence<Shape>()))
{
  return {};
}

template<
    int I
  , class Shape
  , enable_if<is_subshape<Shape>> = 0
  , disable_if<is_static_stride<I, Shape>> = 0
>
IndexInteger get_stride(const Shape& shape) {
  using namespace const_algorithm;
  constexpr int dynamic_index = 
      count(left<I>(StrideSequence<Shape>()), Stride::Dynamic());
  return shape.template dynamic_stride<dynamic_index>();
}

//////////////////////
// get_num_elements //
//////////////////////

template<
    class Shape
  , enable_if<is_static_shape<Shape>> = 0
>
constexpr auto get_num_elements(const Shape& shape) ->
    StaticIndex<
        const_algorithm::product(Dimensionality<Shape>())
    >
{
  return {};
}

namespace detail {

template<
    int I
  , class Shape
  , enable_if_c<I == 0> = 0
>
IndexInteger get_num_elements_impl(const Shape& shape) {
  return get_extent<I>(shape);
}

template<
    int I
  , class Shape
  , enable_if_c<I != 0> = 0
>
IndexInteger get_num_elements_impl(const Shape& shape) {
  return get_extent<I>(shape)*get_num_elements_impl<I-1>(shape);
}

} // end namespace detail

template<
    class Shape
  , enable_if<is_shape<Shape>> = 0
  , disable_if<is_static_shape<Shape>> = 0
>
IndexInteger get_num_elements(const Shape& shape) {
  return detail::get_num_elements_impl<get_rank<Shape>()-1>(shape);
}

//////////////////
// get_1d_index //
//////////////////

namespace detail {

template<
    int I
  , class Shape
  , enable_if<is_contiguous_shape<Shape>> = 0
>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i) {
  return i;
}

template<
    int I
  , class Shape
  , class... Indexes
  , enable_if<is_contiguous_shape<Shape>> = 0
>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i, Indexes... i_rest) {
  return i + get_extent<I>(shape)*get_1d_index_impl<I+1>(shape, i_rest...);
}

template<
    int I
  , class Shape
  , enable_if<is_subshape<Shape>> = 0
>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i) {
  return get_stride<I>(shape)*i;
}

template<
    int I
  , class Shape
  , class... Indexes
  , enable_if<is_subshape<Shape>> = 0
>
IndexInteger get_1d_index_impl(const Shape& shape, IndexInteger i, Indexes... i_rest) {
  return get_stride<I>(shape)*i + get_1d_index_impl<I+1>(shape, i_rest...);
}

} //end namespace detail

template<
    class Shape
  , class... Indexes
  , enable_if<is_shape<Shape>> = 0
>
Index<1> get_1d_index(const Shape& shape, Indexes... indexes) {
  static_assert(sizeof...(Indexes) == get_rank<Shape>()
              , "number of indexes must match the rank of the dimensionality");
  static_assert(
      const_algorithm::and_(
          fatal::constant_sequence<bool, std::is_convertible<Indexes, IndexInteger>::value...>()
      )
    , "indexes must be convertible to IndexInteger");
  return Index<1>{detail::get_1d_index_impl<0>(shape, indexes...)};
};

//////////////////
// get_nd_index //
//////////////////

////////////////
// operator== //
////////////////

namespace detail {

template<class Index1, class Index2>
struct AreDimensionalitiesStaticallyUnequal {
  static const bool value = false;
};

template<
    IndexInteger Extent1
  , IndexInteger Extent2
>
struct AreDimensionalitiesStaticallyUnequal<
    StaticIndex<Extent1>
  , StaticIndex<Extent2>
> {
  static const bool value = 
          Extent1 != Dimension::kDynamic 
       && Extent2 != Dimension::kDynamic
       && Extent1 != Extent2;
};

template<
    IndexInteger ExtentFirst1
  , IndexInteger ExtentFirst2
  , IndexInteger... ExtentsRest1
  , IndexInteger... ExtentsRest2
>
struct AreDimensionalitiesStaticallyUnequal<
    StaticIndex<ExtentFirst1, ExtentsRest1...>
  , StaticIndex<ExtentFirst2, ExtentsRest2...>
> {
  static const bool value = 
          (ExtentFirst1 != Dimension::kDynamic 
       && ExtentFirst2 != Dimension::kDynamic
       && ExtentFirst1 != ExtentFirst2)
       || AreDimensionalitiesStaticallyUnequal<
              StaticIndex<ExtentsRest1...>
            , StaticIndex<ExtentsRest2...>
          >::value;
};

template<
    class Shape1
  , class Shape2
  , enable_if_c<get_rank<Shape1>() != get_rank<Shape2>()> = 0
>
constexpr bool are_shapes_statically_unequal() {
  return true;
}

template<
    class Shape1
  , class Shape2
  , disable_if_c<get_rank<Shape1>() != get_rank<Shape2>()> = 0
  , enable_if<
        AreDimensionalitiesStaticallyUnequal<
            Dimensionality<Shape1>
          , Dimensionality<Shape2>
        >
    > = 0
>
constexpr bool are_shapes_statically_unequal() {
  return true;
}

template<
    class Shape1
  , class Shape2
  , disable_if_c<get_rank<Shape1>() != get_rank<Shape2>()> = 0
  , disable_if<
        AreDimensionalitiesStaticallyUnequal<
            Dimensionality<Shape1>
          , Dimensionality<Shape2>
        >
    > = 0
>
constexpr bool are_shapes_statically_unequal() {
  return false;
}

template<
    int I
  , class Shape1
  , class Shape2
  , enable_if_c<I == get_rank<Shape1>()> = 0
>
constexpr bool are_shapes_equal(const Shape1&, const Shape2&) {
  return true;
}

template<
    int I
  , class Shape1
  , class Shape2
  , enable_if_c<I != get_rank<Shape1>()> = 0
>
bool are_shapes_equal(const Shape1& shape1, const Shape2& shape2) {
  return get_extent<I>(shape1) == get_extent<I>(shape2)
      && are_shapes_equal<I+1>(shape1, shape2);
}

} //end namespace detail

template<
    class Shape1
  , class Shape2
  , enable_if<is_shape<Shape1>> = 0
  , enable_if<is_shape<Shape2>> = 0
  , enable_if_c<detail::are_shapes_statically_unequal<Shape1, Shape2>()> = 0
>
constexpr std::false_type operator==(const Shape1&, const Shape2&) {
  return {};
}
template<
    class Shape1
  , class Shape2
  , enable_if_c<is_static_shape<Shape1>::value && is_static_shape<Shape2>::value> = 0
  , enable_if<std::is_same<Shape1, Shape2>> = 0
>
constexpr std::true_type operator==(const Shape1, const Shape2&) {
  return {};
}
template<
    class Shape1
  , class Shape2
  , enable_if<is_shape<Shape1>> = 0
  , enable_if<is_shape<Shape2>> = 0
  , disable_if_c<is_static_shape<Shape1>::value && is_static_shape<Shape2>::value> = 0
  , disable_if_c<detail::are_shapes_statically_unequal<Shape1, Shape2>()> = 0
>
bool operator==(const Shape1& shape1, const Shape2& shape2) {
  return detail::are_shapes_equal<0>(shape1, shape2);
}

////////////////
// operator!= //
////////////////

template<
    class Shape1
  , class Shape2
  , enable_if<is_shape<Shape1>> = 0
  , enable_if<is_shape<Shape2>> = 0
  , enable_if<
        std::is_same<
            decltype(std::declval<Shape1>() == std::declval<Shape2>())
          , std::true_type
        >
    > = 0
>
constexpr std::false_type operator!=(const Shape1& lhs, const Shape2& rhs) {
  return {};
}

template<
    class Shape1
  , class Shape2
  , enable_if<is_shape<Shape1>> = 0
  , enable_if<is_shape<Shape2>> = 0
  , enable_if<
        std::is_same<
            decltype(std::declval<Shape1>() == std::declval<Shape2>())
          , std::false_type
        >
    > = 0
>
constexpr std::true_type operator!=(const Shape1& lhs, const Shape2& rhs) {
  return {};
}

template<
    class Shape1
  , class Shape2
  , enable_if<is_shape<Shape1>> = 0
  , enable_if<is_shape<Shape2>> = 0
  , enable_if<
        std::is_same<
            decltype(std::declval<Shape1>() == std::declval<Shape2>())
          , bool
        >
    > = 0
>
bool operator!=(const Shape1& lhs, const Shape2& rhs) {
  return !(lhs == rhs);
}

}} //end namespace
