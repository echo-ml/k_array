#pragma once

#include <echo/k_array/shape_traits.h>
#include <echo/concept.h>

namespace echo {
namespace k_array_traits {

//------------------------------------------------------------------------------
// pointer
//------------------------------------------------------------------------------
template <class KArray>
using pointer = decltype(std::declval<KArray>().data());

//------------------------------------------------------------------------------
// const_pointer
//------------------------------------------------------------------------------
template <class KArray>
using const_pointer = decltype(std::declval<KArray>().const_data());

//------------------------------------------------------------------------------
// value_type
//------------------------------------------------------------------------------
template <class KArray>
using value_type = iterator_traits::value_type<pointer<KArray>>;
}
}
