#pragma once

namespace echo { namespace shape_traits {

////////////////////
// dimensionality //
////////////////////

template<class Shape>
using dimensionality = typename Shape::Dimensionality;

/////////////////////
// stride_sequence //
/////////////////////

template<class Shape>
using stride_sequence = typename Shape::StrideSequence;

}  // namespace shape_traits
}  // namespace echo
