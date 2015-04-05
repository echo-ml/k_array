#pragma once

#include <echo/k_array/dimensionality.h>
#include <echo/concept2.h>
#include <echo/const_algorithm.h>

namespace echo {
namespace k_array {
namespace concept {

using echo::concept::Concept;
using echo::concept::models;

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
}
}

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
      -> list<
              dimensionality<typename T::Dimensionality>(),
              stride_sequence<typename T::StrideSequence>(),
              valid<decltype(shape.template dynamic_extent<0>())>(),
              valid<decltype(shape.template dynamic_stride<0>())>(),
              T::Dimensionality::size == T::StrideSequence::size
              >;
};
}
}

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
}
}

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

}
}

template <class T>
constexpr bool static_shape() {
  return models<detail::concept::StaticShape, T>();
}

/////////////
// k_array //
/////////////

namespace detail { namespace concept {

struct KArray : Concept {
  template<class KArray>
  auto require(KArray&& x) -> list<
      echo::concept::contiguous_iterator<decltype(x.data())>()
      ,shape<uncvref_t<decltype(x.shape())>>()
  >;
};

}}

template<class T>
constexpr bool k_array() {
  return models<detail::concept::KArray, T>();
}

////////////////////////
// contiguous_k_array //
////////////////////////

namespace detail { namespace concept {

struct ContiguousKArray : Concept {
  template<class KArray>
  auto require(KArray&& x) -> list<
      k_array<KArray>(),
      contiguous_shape<uncvref_t<decltype(x.shape())>>()
  >;
};

}}

template<class T>
constexpr bool contiguous_k_array() {
  return models<detail::concept::ContiguousKArray, T>();
}

}
}
}
