#pragma once

#include <echo/k_array/shape.h>
#include <echo/k_array/shaped_expression.h>
#include <utility>

namespace echo { namespace k_array {

////////////////
// is_k_array //
////////////////

TICK_TRAIT(is_k_array)
{
  template<class T>
  auto requires_(const T& k_array) -> tick::valid<
      decltype(k_array.data())
    , decltype(k_array.shape())
    , is_true<is_shape<decltype(k_array.shape())>>
  >;
};

////////////////////
// k_array_traits //
////////////////////

namespace k_array_traits {

template<class KArray>
using Pointer = decltype(std::declval<KArray>().data());

template<class KArray>
using Shape = typename std::decay<decltype(std::declval<KArray>().shape())>::type;

template<class KArray>
using Dimensionality = typename Shape<KArray>::Dimensionality;

} //end namespace k_array_traits

}} //end namespace echo::k_array
