#pragma once

#define DETAIL_NS detail_concept

#include <echo/k_array/k_array_fwd.h>
#include <echo/k_array/k_array_view_fwd.h>
#include <echo/index.h>
#include <echo/concept.h>
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

//------------------------------------------------------------------------------
// extent
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <index_t I>
auto extent_impl(StaticIndex<I>) -> std::true_type;

inline auto extent_impl(index_t) -> std::true_type;

inline auto extent_impl(...) -> std::false_type;
}

template <class T>
constexpr bool extent() {
  using Result = decltype(DETAIL_NS::extent_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// static_extent
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <index_t I>
auto static_extent_impl(StaticIndex<I> && ) -> std::true_type;

template <class T>
auto static_extent_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool static_extent() {
  using Result =
      decltype(DETAIL_NS::static_extent_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// dynamic_extent
//------------------------------------------------------------------------------
template <class T>
constexpr bool dynamic_extent() {
  return !static_extent<T>() && extent<T>();
}

//------------------------------------------------------------------------------
// index_tuple
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class... Extents, CONCEPT_REQUIRES(and_c<extent<Extents>()...>())>
auto index_tuple_impl(htl::Tuple<Extents...> && ) -> std::true_type;

template <class T>
auto index_timple_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool index_tuple() {
  using Result = decltype(DETAIL_NS::index_tuple_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// dimensionality
//------------------------------------------------------------------------------
namespace DETAIL_NS {
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

template <class T>
constexpr bool dimensionality() {
  using Result =
      decltype(DETAIL_NS::dimensionality_impl(std::declval<T>()));
  return Result::value;
}

template <class T>
constexpr bool static_dimensionality() {
  using Result =
      decltype(DETAIL_NS::static_dimensionality_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// contiguous_shape
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class... Extents>
auto contiguous_shape_impl(Shape<Extents...> && ) -> std::true_type;

template <class T>
auto contiguous_shape_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool contiguous_shape() {
  using Result =
      decltype(DETAIL_NS::contiguous_shape_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// subshape
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Dimensionality, class Strides>
auto subshape_impl(Subshape<Dimensionality, Strides> && ) -> std::true_type;

template <class T>
auto subshape_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool subshape() {
  using Result = decltype(DETAIL_NS::subshape_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// shape
//------------------------------------------------------------------------------
template <class T>
constexpr bool shape() {
  return contiguous_shape<T>() || subshape<T>();
}

//------------------------------------------------------------------------------
// static_shape
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct StaticShape : Concept {
  template <class T>
  auto require(T&& shape) -> list<
      k_array::concept::shape<T>(),
      static_dimensionality<uncvref_t<decltype(shape.dimensionality())>>()>;
};
}

template <class T>
constexpr bool static_shape() {
  return models<DETAIL_NS::StaticShape, T>();
}

//------------------------------------------------------------------------------
// shaped
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct Shaped : Concept {
  template <class T>
  auto require(T&& x) -> list<shape<uncvref_t<decltype(x.shape())>>()>;
};
}

template <class T>
constexpr bool shaped() {
  return models<DETAIL_NS::Shaped, T>();
}

//------------------------------------------------------------------------------
// dimensioned
//------------------------------------------------------------------------------
namespace DETAIL_NS {
struct Dimensioned : Concept {
  template <class T>
  auto require(T&& x)
      -> list<dimensionality<uncvref_t<decltype(x.dimensionality())>>()>;
};
}

template <class T>
constexpr bool dimensioned() {
  return models<DETAIL_NS::Dimensioned, T>() || shaped<T>();
}

//------------------------------------------------------------------------------
// k_array_deep
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class T, class Shape, class Allocator>
auto k_array_deep_impl(KArray<T, Shape, Allocator> && ) -> std::true_type;

template <class T>
auto k_array_deep_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool k_array_deep() {
  using Result =
      decltype(DETAIL_NS::k_array_deep_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// k_array_view
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Pointer, class Shape, class MemoryBackendTag>
auto k_array_view_impl(KArrayView<Pointer, Shape, MemoryBackendTag> && )
    -> std::true_type;

template <class T>
auto k_array_view_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool k_array_view() {
  using Result =
      decltype(DETAIL_NS::k_array_view_impl(std::declval<T>()));
  return Result::value;
}

//------------------------------------------------------------------------------
// k_array
//------------------------------------------------------------------------------
template <class T>
constexpr bool k_array() {
  return k_array_deep<T>() || k_array_view<T>();
}
}
}
}

#undef DETAIL_NS
