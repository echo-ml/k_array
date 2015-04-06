#pragma once

#include <echo/k_array/shape.h>

namespace echo {
namespace k_array {

//////////////////////
// get_num_elements //
//////////////////////

template <class Expression, CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_num_elements(const Expression& expression) {
  return get_num_elements(expression.shape());
}

////////////////////////
// get_num_dimensions //
////////////////////////

template <class Expression, CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_num_dimensions() {
  using Shape =
      typename std::decay<decltype(std::declval<Expression>().shape())>::type;
  return get_num_dimensions<Shape>();
}

////////////////
// get_extent //
////////////////

template <int I, class Expression,
          CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_extent(const Expression& expression) {
  using Shape =
      typename std::decay<decltype(std::declval<Expression>().shape())>::type;
  return get_extent<I>(expression.shape());
}

////////////////
// get_stride //
////////////////

template <int I, class Expression,
          CONCEPT_REQUIRES(concept::shaped<Expression>())>
constexpr auto get_stride(const Expression& expression) {
  using Shape =
      typename std::decay<decltype(std::declval<Expression>().shape())>::type;
  return get_stride<I>(expression.shape());
}

//////////////////////
// get_first_shaped //
//////////////////////

template <class TFirst, class... TRest,
          CONCEPT_REQUIRES(concept::shaped<TFirst>())>
const auto& get_first_shaped(const TFirst& x_first, const TRest&... x_rest) {
  return x_first.shape();
}

template <class TFirst, class... TRest,
          CONCEPT_REQUIRES(!concept::shaped<TFirst>())>
const auto& get_first_shaped(const TFirst& x_first, const TRest&... x_rest) {
  static_assert(sizeof...(TRest) > 0, "arguments contain no shape");
  return get_first_shaped(x_rest...);
}

/////////////////////////////
// assert_any_shaped_match //
/////////////////////////////

namespace detail {
namespace shaped_expression {

template <class Shaped>
void assert_any_shaped_match_impl(const Shaped&) {}

template <class Shape, class ShapedFirst, class... ShapedRest,
          CONCEPT_REQUIRES(!concept::shaped<ShapedFirst>())>
void assert_any_shaped_match_impl(const Shape& shape,
                                  const ShapedFirst& shaped_first,
                                  const ShapedRest&... shaped_rest);
template <class Shape, class ShapedFirst, class... ShapedRest,
          CONCEPT_REQUIRES(concept::shaped<ShapedFirst>())>
void assert_any_shaped_match_impl(const Shape& shape,
                                  const ShapedFirst& shaped_first,
                                  const ShapedRest&... shaped_rest) {
  ECHO_ASSERT(k_array::operator==(shape, shaped_first.shape()));
  assert_any_shaped_match_impl(shape, shaped_rest...);
}

template <class Shape, class ShapedFirst, class... ShapedRest,
          CONCEPT_REQUIRES_REDECLARATION(!concept::shaped<ShapedFirst>())>
void assert_any_shaped_match_impl(const Shape& shape,
                                  const ShapedFirst& shaped_first,
                                  const ShapedRest&... shaped_rest) {
  assert_any_shaped_match_impl(shape, shaped_rest...);
}
}  // namespace shaped_expression
}  // end namespace detail

template <class... Shaped>
void assert_any_shaped_match(const Shaped&... shaped) {
  detail::shaped_expression::assert_any_shaped_match_impl(
      get_first_shaped(shaped...), shaped...);
}
}  // namespace k_array
}  // namespace echo
