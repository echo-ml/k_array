#pragma once

#include <echo/k_array/concept.h>
#include <echo/utility/range.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// all_begin
//------------------------------------------------------------------------------
template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_begin(KArray&& k_array) {
  return k_array.data();
}

//------------------------------------------------------------------------------
// all_end
//------------------------------------------------------------------------------
template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_end(KArray&& k_array) {
  return all_begin(k_array) + get_num_elements(k_array);
}

//------------------------------------------------------------------------------
// all_cbegin
//------------------------------------------------------------------------------
template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_cbegin(KArray&& k_array) {
  return k_array.const_data();
}

//------------------------------------------------------------------------------
// all_cend
//------------------------------------------------------------------------------
template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_cend(KArray&& k_array) {
  return all_cbegin(k_array) + get_num_elements(k_array);
}

//------------------------------------------------------------------------------
// all_range
//------------------------------------------------------------------------------
template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_range(KArray&& k_array) {
  return make_range(all_begin(k_array), all_end(k_array));
}

//------------------------------------------------------------------------------
// all_crange
//------------------------------------------------------------------------------
template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto all_crange(KArray&& k_array) {
  return make_range(all_cbegin(k_array), all_cend(k_array));
}
}
}
