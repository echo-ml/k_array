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
  return shape.template dynamic_extent<
      count(left<I>(Dimensionality<Shape>()), Dimension::Dynamic())
  >();
}

//////////////////////
// is_static_stride //
//////////////////////

template<
    int I
  , class Shape
  , enable_if<is_contiguous_shape<Shape>> = 0
>
constexpr bool is_static_stride() {
  using namespace const_algorithm;
  return !contains(left<I>(Dimensionality<Shape>()), Dimension::Dynamic());
}

template<
    int I
  , class Shape
  , enable_if<is_subshape<Shape>> = 0
>
constexpr bool is_static_stride() {
  return get<I>(StrideSequence<Shape>()) != Stride::kDynamic;
}

////////////////
// get_stride //
////////////////

template<
    int I
  , class Shape
  , enable_if<is_contiguous_shape<Shape>> = 0
  , enable_if_c<is_static_stride<I, Shape>()> = 0
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
  , disable_if_c<is_static_stride<I, Shape>()> = 0
>
constexpr IndexInteger get_stride(const Shape& shape) {
  return get_extent<I-1>(shape) * get_stride<I-1>(shape);
}

template<
    int I
  , class Shape
  , enable_if<is_subshape<Shape>> = 0
  , enable_if_c<is_static_stride<I, Shape>()> = 0
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
  , disable_if_c<is_static_stride<I, Shape>()> = 0
>
IndexInteger get_stride(const Shape& shape) {
  using namespace const_algorithm;
  return shape.template dynamic_stride<
      count(left<I>(StrideSequence<Shape>()), Stride::Dynamic())
  >();
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

}} //end namespace
