#pragma once

#include <echo/k_array/shape.h>
#include <utility>

namespace echo { namespace k_array {

TICK_TRAIT(is_k_array
         , is_contiguous_shape<_>)
{
  template<class T>
  auto requires_(const T& k_array) -> tick::valid<
      decltype(k_array.data())
    , decltype(k_array.shape())
  >;
};

namespace k_array_traits {

template<class KArray>
using Pointer = decltype(std::declval<KArray>().data());

template<class KArray>
using Shape = typename std::decay<decltype(std::declval<KArray>().shape())>::type;

} //end namespace k_array_traits

}} //end namespace echo::k_array
