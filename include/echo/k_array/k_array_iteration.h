#pragma once

#include <echo/k_array/concept.h>
#include <iterator>

#include <boost/range.hpp>

namespace echo {
namespace k_array {

template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_begin(KArray&& k_array) {
  return k_array.data();
}

template <class KArray, CONCEPT_REQUIRES(concept::contiguous_k_array<KArray>())>
auto all_cbegin(const KArray& k_array) {
  return k_array.data();
}

template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_end(KArray&& k_array) {
  return k_array.data() + get_num_elements(k_array);
}

template <class KArray, CONCEPT_REQUIRES(concept::contiguous_k_array<KArray>())>
auto all_cend(const KArray& k_array) {
  return k_array.data() + get_num_elements(k_array);
}

template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_range(KArray&& k_array) {
  return boost::make_iterator_range(all_begin(k_array), all_end(k_array));
}

template <class KArray, CONCEPT_REQUIRES(concept::contiguous_k_array<KArray>())>
auto all_crange(const KArray& k_array) {
  return boost::make_iterator_range(all_begin(k_array), all_end(k_array));
}

}  // namespace k_array
}  // namespace echo

