#pragma once

#include <echo/k_array/shape.h>
#include <echo/k_array/shape_traits.h>
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
using shape_type = uncvref_t<decltype(std::declval<KArray>().shape())>;

template <int I, class KArray>
using extent_type = shape_traits::extent_type<I, shape_type<KArray>>;
    // decltype(k_array::get_extent<I>(std::declval<shape_type<KArray>>()));

template <class KArray>
using dimensionality = shape_traits::dimensionality<shape_type<KArray>>;
}  // namespace k_array_traits
}  // namespace echo
