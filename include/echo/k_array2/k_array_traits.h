#pragma once

#include <echo/k_array2/shape_traits.h>
#include <echo/concept2.h>

namespace echo {
namespace k_array_traits {

template <class KArray>
using pointer = decltype(std::declval<KArray>().data());

template <class KArray>
using const_pointer = decltype(std::declval<KArray>().const_data());

template <class KArray>
using value_type = iterator_traits::value_type<pointer<KArray>>;

template <class KArray>
using dimensionality_type =
    uncvref_t<decltype(std::declval<KArray>().shape().dimensionality())>;

template <class KArray>
using shape_type = uncvref_t<decltype(std::declval<KArray>().shape())>;

template <class KArray>
constexpr auto num_dimensions()
    -> decltype(shape_traits::num_dimensions<shape_type<KArray>>()) {
  return shape_traits::num_dimensions<shape_type<KArray>>();
}

template<int I, class KArray>
using extent_type = shape_traits::extent_type<I, shape_type<KArray>>;

template<int I, class KArray>
using stride_type = shape_traits::stride_type<I, shape_type<KArray>>;
}
}
