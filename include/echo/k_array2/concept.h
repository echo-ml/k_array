#pragma once

#include <echo/index.h>
#include <echo/concept2.h>
#include <echo/htl.h>

namespace echo {
namespace k_array {

template <class... Extents>
class Dimensionality;

template <class... Extents>
class Shape;

template <class Dimensionality, class Strides>
class Subshape;

namespace concept {

//////////////////
// static_index //
//////////////////

namespace detail {
namespace concept {

template <index_t I>
auto static_index_impl(StaticIndex<I> && ) -> std::true_type;

template <class T>
auto static_index_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool static_index() {
  using Result =
      decltype(detail::concept::static_index_impl(std::declval<T>()));
  return Result::value;
}

////////////
// extent //
////////////

namespace detail {
namespace concept {

template <index_t I>
auto extent_impl(StaticIndex<I>) -> std::true_type;

inline auto extent_impl(index_t) -> std::true_type;

inline auto extent_impl(...) -> std::false_type;
}
}

template <class T>
constexpr bool extent() {
  using Result = decltype(detail::concept::extent_impl(std::declval<T>()));
  return Result::value;
}

/////////////////
// index_tuple //
/////////////////

namespace detail {
namespace concept {

template <class... Extents, CONCEPT_REQUIRES(and_c<extent<Extents>()...>())>
auto index_tuple_impl(htl::Tuple<Extents...> && ) -> std::true_type;

template <class T>
auto index_timple_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool index_tuple() {
  using Result = decltype(detail::concept::index_tuple_impl(std::declval<T>()));
  return Result::value;
}

////////////////////
// dimensionality //
////////////////////

namespace detail {
namespace concept {
template <class... Extents, CONCEPT_REQUIRES(and_c<extent<Extents>()...>())>
auto dimensionality_impl(Dimensionality<Extents...> && ) -> std::true_type;

template <class T>
auto dimensionality_impl(T && ) -> std::false_type;

template <index_t... Indexes>
auto static_dimensionality_impl(Dimensionality<StaticIndex<Indexes>...> && )
    -> std::true_type;

template <class T>
auto static_dimensionality_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool dimensionality() {
  using Result =
      decltype(detail::concept::dimensionality_impl(std::declval<T>()));
  return Result::value;
}

template <class T>
constexpr bool static_dimensionality() {
  using Result =
      decltype(detail::concept::static_dimensionality_impl(std::declval<T>()));
  return Result::value;
}

//////////////////////
// contiguous_shape //
//////////////////////

namespace detail {
namespace concept {

template <class... Extents>
auto contiguous_shape_impl(Shape<Extents...> && ) -> std::true_type;

template <class T>
auto contiguous_shape_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool contiguous_shape() {
  using Result =
      decltype(detail::concept::contiguous_shape_impl(std::declval<T>()));
  return Result::value;
}

//////////////
// subshape //
//////////////

namespace detail {
namespace concept {

template <class Dimensionality, class Strides>
auto subshape_impl(Subshape<Dimensionality, Strides> && ) -> std::true_type;

template <class T>
auto subshape_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool subshape() {
  using Result = decltype(detail::concept::subshape_impl(std::declval<T>()));
  return Result::value;
}

///////////
// shape //
///////////

template <class T>
constexpr bool shape() {
  return contiguous_shape<T>() || subshape<T>();
}

//////////////////
// static_shape //
//////////////////

namespace detail {
namespace concept {

struct StaticShape : Concept {
  template <class T>
  auto require(T&& shape) -> list<
      k_array::concept::shape<T>(),
      static_dimensionality<uncvref_t<decltype(shape.dimensionality())>>()>;
};
}
}

template <class T>
constexpr bool static_shape() {
  return models<detail::concept::StaticShape, T>();
}

/////////////////
// dimensioned //
/////////////////

namespace detail {
namespace concept {

struct Dimensioned : Concept {
  template <class T>
  auto require(T&& x) -> list<dimensionality<decltype(x.dimensionality())>()>;
};
}
}

template <class T>
constexpr bool dimensioned() {
  return models<detail::concept::Dimensioned, T>();
}

////////////
// shaped //
////////////

namespace detail {
namespace concept {

struct Shaped : Concept {
  template <class T>
  auto require(T&& x) -> list<shape<decltype(x.shape())>()>;
};
}
}

template <class T>
constexpr bool shaped() {
  return models<detail::concept::Shaped, T>();
}
}
}
}
