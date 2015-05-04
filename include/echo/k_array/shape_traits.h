#pragma once

#include <echo/k_array/dimensionality.h>
#include <echo/index.h>
#include <echo/concept2.h>

namespace echo {
namespace shape_traits {

////////////////////
// dimensionality //
////////////////////

template <class Shape>
using dimensionality = typename Shape::Dimensionality;

/////////////////////
// stride_sequence //
/////////////////////

template <class Shape>
using stride_sequence = typename Shape::StrideSequence;

////////////////////
// num_dimensions //
////////////////////

template <class Shape>
constexpr auto num_dimensions()
    -> decltype(std::decay<Shape>::type::Dimensionality::size, index_t()) {
  return std::decay<Shape>::type::Dimensionality::size;
}

///////////////////
// static_extent //
///////////////////

template <int I, class Shape>
constexpr bool static_extent() {
  return get<I>(dimensionality<Shape>()) != k_array::Dimension::kDynamic;
}

///////////////////
// static_stride //
///////////////////

namespace detail {
namespace shape_traits {

template <int I, class Shape, bool IsSubshape>
struct IsStaticStrideImpl {};

template <int I, class Shape>
struct IsStaticStrideImpl<I, Shape, false>
    : std::integral_constant<bool, !const_algorithm::contains(
                                       const_algorithm::left<I>(
                                           dimensionality<Shape>()),
                                       k_array::Dimension::Dynamic())> {};

template <int I, class Shape>
struct IsStaticStrideImpl<I, Shape, true>
    : std::integral_constant<bool, get<I>(stride_sequence<Shape>()) !=
                                       k_array::Stride::kDynamic> {};

ECHO_MAKE_TYPE_QUERIER(StrideSequence)

}  // namespace shape_traits
}  // end namespace detail

template <int I, class Shape>
constexpr bool static_stride() {
  using Result = detail::shape_traits::IsStaticStrideImpl<
      I, Shape, detail::shape_traits::has_StrideSequence<Shape>()>;
  return Result::value;
}

}  // namespace shape_traits
}  // namespace echo
