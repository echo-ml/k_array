#pragma once

#include <echo/k_array/shape.h>
#include <iterator>

namespace echo { namespace k_array {

#define MAKE_ACCESSOR(QUALIFIER_NAME, QUALIFIER) \
  template<class Derived, class Pointer> \
  struct KArray ## QUALIFIER_NAME ## Accessor { \
    template<class... Indexes> \
    typename std::iterator_traits<Pointer>::reference \
    operator()(Indexes... indexes) QUALIFIER \
    { \
      QUALIFIER Derived& derived = static_cast<QUALIFIER Derived&>(*this); \
      return *(derived.data() + get_1d_index(derived, indexes...)); \
    } \
    typename std::iterator_traits<Pointer>::reference \
    operator[](echo::Index<1> index) QUALIFIER { \
      QUALIFIER Derived& derived = static_cast<QUALIFIER Derived&>(*this); \
      return *(derived.data() + index); \
    } \
  };

MAKE_ACCESSOR(/*none*/,/*none*/)
MAKE_ACCESSOR(Const, const)

#undef MAKE_ACCESSOR

}} //end namespace echo::k_array
