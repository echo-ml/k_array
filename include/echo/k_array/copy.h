#pragma once

#include <echo/k_array/concept.h>
#include <echo/k_array/k_array_iteration.h>

namespace echo {
namespace k_array {

template <class SourceKArray, class DestinationKArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<SourceKArray>() &&
                           concept::contiguous_k_array<DestinationKArray>())>
void copy(const SourceKArray& source, DestinationKArray&& destination) {
  ECHO_ASSERT(source.shape() == destination.shape());
  std::copy(all_cbegin(source), all_cend(source), all_begin(destination));
}

}  // namespace k_array
}  // namespace echo
