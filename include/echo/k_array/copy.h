#pragma once

#include <echo/k_array/k_array_concept.h>
#include <echo/k_array/k_array_iteration.h>

namespace echo { namespace k_array {

template<
    class SourceKArray
  , class DestinationKArray
  , enable_if<is_k_array<SourceKArray>> = 0
  , enable_if<is_k_array<DestinationKArray>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<SourceKArray>>> = 0
  , enable_if<is_contiguous_shape<k_array_traits::Shape<DestinationKArray>>> = 0
>
void copy(const SourceKArray& source, DestinationKArray&& destination) {
  ECHO_ASSERT(source.shape() == destination.shape());
  std::copy(all_cbegin(source), all_cend(source), all_begin(destination));
}

}} //end namesapce echo::k_array
