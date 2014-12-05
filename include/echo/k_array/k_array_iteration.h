#pragma once

#include <echo/k_array/k_array_concept.h>
#include <iterator>

namespace echo { namespace k_array {

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
>
auto all_begin(KArray&& k_array) {
  return k_array.data();
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
>
auto all_end(KArray&& k_array) {
  return k_array.data() + get_num_elements(k_array);
}

}} //end namespace echo::k_array

