#pragma once

#include <echo/k_array/concept.h>
#include <echo/k_array/dimensioned.h>
#include <echo/memory.h>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// copy
//------------------------------------------------------------------------------
template <
    class SourceKArray, class DestinationKArray,
    CONCEPT_REQUIRES(
        concept::contiguous_k_array<uncvref_t<SourceKArray>>() &&
        concept::modifiable_k_array_forward<uncvref_t<DestinationKArray>>() &&
        concept::contiguous_k_array<uncvref_t<DestinationKArray>>() &&
        concept::compatible_k_arrays<uncvref_t<SourceKArray>,
                                     uncvref_t<DestinationKArray>>())>
void copy(SourceKArray&& source_k_array,
          DestinationKArray&& destination_k_array) {
  using SourceMemoryBackendTag =
      typename uncvref_t<SourceKArray>::memory_backend_tag;
  using DestinationMemoryBackendTag =
      typename uncvref_t<DestinationKArray>::memory_backend_tag;
  memory::copy(SourceMemoryBackendTag(), source_k_array.const_data(),
               source_k_array.const_data() + get_num_elements(source_k_array),
               DestinationMemoryBackendTag(), destination_k_array.data());
}
}
}
