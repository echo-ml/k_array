#pragma once

#include <echo/k_array/k_array_concept.h>
#include <iterator>

#include <boost/range.hpp>

namespace echo { namespace k_array {

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<KArray>>> = 0
>
auto all_begin(KArray&& k_array) {
  return k_array.data();
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<KArray>>> = 0
>
auto all_cbegin(const KArray& k_array) {
  return k_array.data();
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<KArray>>> = 0
>
auto all_end(KArray&& k_array) {
  return k_array.data() + get_num_elements(k_array);
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<KArray>>> = 0
>
auto all_cend(const KArray& k_array) {
  return k_array.data() + get_num_elements(k_array);
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<KArray>>> = 0
>
auto all_range(KArray&& k_array) {
  return boost::make_iterator_range(all_begin(k_array), all_end(k_array));
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<KArray>>> = 0
>
auto all_crange(const KArray& k_array) {
  return boost::make_iterator_range(all_begin(k_array), all_end(k_array));
}

}} //end namespace echo::k_array

