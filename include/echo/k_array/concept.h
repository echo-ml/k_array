#pragma once

#include <echo/k_array/dimensionality.h>
#include <echo/concept2.h>
#include <echo/const_algorithm.h>

namespace echo {
namespace k_array {
namespace concept {

using echo::concept::Concept;
using echo::concept::models;

///////////////////
// static_extent //
///////////////////

namespace detail {
namespace concept {
template <IndexInteger N, CONCEPT_REQUIRES(N > 1)>
auto static_extent_impl(StaticIndex<N>) -> std::true_type;

auto static_extent_impl(...) -> std::false_type;
}
}

template <class T>
constexpr bool static_extent() {
  using Result =
      decltype(detail::concept::static_extent_impl(std::declval<T>()));
  return Result::value;
}

////////////////////
// dynamic_extent //
////////////////////

template <class T>
constexpr bool dynamic_extent() {
  return std::is_convertible<T, IndexInteger>::value && !static_extent<T>();
}

////////////
// extent //
////////////

template <class T>
constexpr bool extent() {
  return static_extent<T>() || dynamic_extent<T>();
}

////////////////////
// dimensionality //
////////////////////

namespace detail {
namespace concept {
template <IndexInteger... Values>
auto dimensionality_impl(StaticIndex<Values...>) -> std::integral_constant<
    bool, const_algorithm::and_(fatal::constant_sequence<
              bool, (Values > 0) || (Values == Dimension::Dynamic())...>()) &&
              (sizeof...(Values) > 0)>;

inline std::false_type dimensionality_impl(...);
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool dimensionality() {
  return decltype(detail::concept::dimensionality_impl(std::declval<T>())){};
}

/////////////////////
// stride_sequence //
/////////////////////

template <class T>
constexpr bool stride_sequence() {
  return dimensionality<T>();
}

//////////////
// subshape //
//////////////

namespace detail {
namespace concept {
struct Subshape : Concept {
  template <class T>
  auto require(T&& shape)
      -> list<dimensionality<typename T::Dimensionality>(),
              stride_sequence<typename T::StrideSequence>(),
              valid<decltype(shape.template dynamic_extent<0>())>(),
              valid<decltype(shape.template dynamic_stride<0>())>(),
              T::Dimensionality::size == T::StrideSequence::size>;
};
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool subshape() {
  return models<detail::concept::Subshape, T>();
}

//////////////////////
// contiguous_shape //
//////////////////////

namespace detail {
namespace concept {
struct ContiguousShape : Concept {
  template <class T>
  auto require(T&& shape) -> list<
      dimensionality<typename T::Dimensionality>(),
      valid<decltype(shape.template dynamic_extent<0>())>(), !subshape<T>()>;
};
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool contiguous_shape() {
  return models<detail::concept::ContiguousShape, T>();
}

///////////
// shape //
///////////

template <class T>
constexpr bool shape() {
  return subshape<T>() || contiguous_shape<T>();
}

/////////////
// k_shape //
/////////////

namespace detail {
namespace concept {
struct KShape : Concept {
  template <class K, class T>
  auto require(K&&, T && )
      -> list<shape<T>(), T::Dimensionality::size == K::value>;
};
}
}

template <int K, class T>
constexpr bool k_shape() {
  return models<detail::concept::KShape, std::integral_constant<int, K>, T>();
}

////////////////////////
// contiguous_k_shape //
////////////////////////

template <int K, class T>
constexpr bool contiguous_k_shape() {
  return contiguous_shape<T>() && k_shape<K, T>();
}

//////////////////
// static_shape //
//////////////////

namespace detail {
namespace concept {
struct StaticShape : Concept {
  template <class T>
  auto require(T && ) -> list<
      shape<T>(), !const_algorithm::contains(typename T::Dimensionality(),
                                             Dimension::Dynamic())>;
};
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool static_shape() {
  return models<detail::concept::StaticShape, T>();
}

////////////
// shaped //
////////////

namespace detail {
namespace concept {
struct Shaped : Concept {
  template <class T>
  auto require(T&& x) -> list<shape<uncvref_t<decltype(x.shape())>>()>;
};
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool shaped() {
  return models<detail::concept::Shaped, T>();
}

///////////////////
// writable_data //
///////////////////

namespace detail {
namespace concept {
struct WritableData : Concept {
  template <class T>
  auto require(T&& x) -> list<echo::concept::writable<decltype(x.data())>()>;
};
}
}

template <class T>
constexpr bool writable_data() {
  return models<detail::concept::WritableData, T>();
}

/////////////
// k_array //
/////////////

namespace detail {
namespace concept {
struct KArray : Concept {
  template <class KArray>
  auto require(KArray&& x)
      -> list<echo::concept::contiguous_iterator<decltype(x.data())>(),
              shape<uncvref_t<decltype(x.shape())>>()>;
};
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool k_array() {
  return models<detail::concept::KArray, T>();
}

////////////////////////
// contiguous_k_array //
////////////////////////

namespace detail {
namespace concept {
struct ContiguousKArray : Concept {
  template <class KArray>
  auto require(KArray&& x) -> list<
      k_array<KArray>(), contiguous_shape<uncvref_t<decltype(x.shape())>>()>;
};
}  // namespace concept
}  // namespace detail

template <class T>
constexpr bool contiguous_k_array() {
  return models<detail::concept::ContiguousKArray, T>();
}

}  // namespace concept
}  // namespace k_array
}  // namespace echo
