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
  static constexpr StaticIndex<-1> Dynamic{};
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
            , (Values > 0) || (Values == Dimension::Dynamic)...
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

//////////////
// is_shape //
//////////////

TICK_TRAIT(is_shape) {
  template<class Shape>
  auto requires_(const Shape& shape) -> tick::valid<
      is_true<is_dimensionality<typename Shape::Dimensionality>>
    , is_true<is_stride_sequence<typename Shape::Strides>>
    , decltype(shape.template dynamic_extent<0>())
    , decltype(shape.template dynamic_stride<0>())
    , is_true_c<Shape::Dimensionality::size == Shape::Strides::size>
  >;
};

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

////////////////
// get_extent //
////////////////

////////////////
// get_stride //
////////////////

//////////////////////
// get_num_elements //
//////////////////////

//////////////////
// get_1d_index //
//////////////////

//////////////////
// get_nd_index //
//////////////////

}} //end namespace
