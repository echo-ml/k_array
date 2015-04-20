#pragma once

#include <echo/k_array/shape.h>
#include <echo/k_array/shaped_expression.h>
#include <utility>

namespace echo {
namespace k_array {

////////////////////
// k_array_traits //
////////////////////

//depricated
namespace k_array_traits {

template <class KArray>
using Pointer = decltype(std::declval<KArray>().data());

template <class KArray>
using Shape =
    typename std::decay<decltype(std::declval<KArray>().shape())>::type;

template <class KArray>
using Dimensionality = typename Shape<KArray>::Dimensionality;

}  // end namespace k_array_traits

}  // namespace k_array
}  // namespace echo
