#include <echo/k_array/shape.h>

namespace echo { namespace k_array {

///////////////
// has_shape //
///////////////

TICK_TRAIT(has_shape) {
  template<class Expr>
  auto requires_(const Expr& expr) -> tick::valid<
      is_shape<decltype(expr.shape())>
  >;
};

//////////////////////
// get_num_elements //
//////////////////////

template<
    class Expression
  , enable_if<has_shape<Expression>> = 0
>
constexpr auto get_num_elements(const Expression& expression) { 
  return get_num_elements(expression.shape());
}

////////////////////////
// get_num_dimensions //
////////////////////////

template<
    class Expression
  , enable_if<has_shape<Expression>> = 0
>
constexpr auto get_num_dimensions() { 
  using Shape = typename std::decay<decltype(std::declval<Expression>().shape())>::type;
  return get_num_dimensions<Shape>();
}

////////////////
// get_extent //
////////////////

template<
    int I
  , class Expression
  , enable_if<has_shape<Expression>> = 0
>
constexpr auto get_extent(const Expression& expression) { 
  using Shape = typename std::decay<decltype(std::declval<Expression>().shape())>::type;
  return get_extent<I>(expression.shape());
}

////////////////
// get_stride //
////////////////

template<
    int I
  , class Expression
  , enable_if<has_shape<Expression>> = 0
>
constexpr auto get_stride(const Expression& expression) { 
  using Shape = typename std::decay<decltype(std::declval<Expression>().shape())>::type;
  return get_stride<I>(expression.shape());
}

//////////////////////
// get_first_shaped //
//////////////////////

template<
    class TFirst
  , class... TRest
  , enable_if<has_shape<TFirst>> = 0
>
const auto& get_first_shaped(const TFirst& x_first, const TRest&... x_rest) {
  return x_first.shape();
}

template<
    class TFirst
  , class... TRest
  , disable_if<has_shape<TFirst>> = 0
>
const auto& get_first_shaped(const TFirst& x_first, const TRest&... x_rest) {
  static_assert(sizeof...(TRest) > 0, "arguments contain no shape");
  return get_first_shaped(x_rest...);
}

/////////////////////////////
// assert_any_shaped_match //
/////////////////////////////

namespace detail {

template<class Shaped>
void assert_any_shaped_match_impl(const Shaped&) {
}

template<
    class Shape
  , class ShapedFirst
  , class... ShapedRest
  , disable_if<has_shape<ShapedFirst>> = 0
>
void assert_any_shaped_match_impl(const Shape& shape
                                , const ShapedFirst& shaped_first
                                , const ShapedRest&... shaped_rest);
template<
    class Shape
  , class ShapedFirst
  , class... ShapedRest
  , enable_if<has_shape<ShapedFirst>> = 0
>
void assert_any_shaped_match_impl(const Shape& shape
                                , const ShapedFirst& shaped_first
                                , const ShapedRest&... shaped_rest)
{
  ECHO_ASSERT(k_array::operator==(shape, shaped_first.shape()));
  assert_any_shaped_match_impl(shape, shaped_rest...);
}

template<
    class Shape
  , class ShapedFirst
  , class... ShapedRest
  , disable_if<has_shape<ShapedFirst>>
>
void assert_any_shaped_match_impl(const Shape& shape
                                , const ShapedFirst& shaped_first
                                , const ShapedRest&... shaped_rest)
{
  assert_any_shaped_match_impl(shape, shaped_rest...);
}

} //end namespace detail

template<class... Shaped>
void assert_any_shaped_match(const Shaped&... shaped) {
  detail::assert_any_shaped_match_impl(get_first_shaped(shaped...), shaped...);
}

}} //end namespace echo::k_array
