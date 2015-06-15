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
}
}
