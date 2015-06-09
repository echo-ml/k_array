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

struct IndexTuple : Concept {
  template <class... Extents>
  auto require(htl::Tuple<Extents...>&& tuple)
      -> list<and_c<extent<Extents>()...>()>;
};
}
}

template <class T>
constexpr bool index_tuple() {
  return models<detail::concept::IndexTuple, T>();
}

////////////////////
// dimensionality //
////////////////////

namespace detail {
namespace concept {
template <class... Extents,
          CONCEPT_REQUIRES(and_c<extent<Extents>()...>())>
auto dimensionality_impl(Dimensionality<Extents...> && ) -> std::true_type;

template <class T>
auto dimensionality_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool dimensionality() {
  using Result =
      decltype(detail::concept::dimensionality_impl(std::declval<T>()));
  return Result::value;
}

//////////////
// subshape //
//////////////

namespace detail { namespace concept {

template<class Dimensionality, class Strides>
auto subshape_impl(Subshape<Dimensionality, Strides>&&)  -> std::true_type;

template<class T>
auto subshape_impl(T&&) -> std::false_type;

}}

template<class T>
constexpr bool subshape() {
  using Result =
    decltype(detail::concept::subshape_impl(std::declval<T>()));
  return Result::value;
}

}
}
}
