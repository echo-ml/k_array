#pragma once

#include <echo/k_array/shape.h>

#include <initializer_list>
#include <algorithm>

namespace echo { namespace k_array {

#define MAKE_ASSIGNMENT(QUALIFIER_NAME, QUALIFIER) \
  template<class Derived, class T> \
  struct KArray ## QUALIFIER_NAME ## Assignment { \
    template<class... Indexes> \
    QUALIFIER Derived& \
    operator=(std::initializer_list<T> values) QUALIFIER \
    { \
      QUALIFIER Derived& derived = static_cast<QUALIFIER Derived&>(*this); \
      assert(values.size() == get_num_elements(derived)); \
      std::copy(values.begin(), values.end(), derived.data()); \
      return derived; \
    } \
  };

MAKE_ASSIGNMENT(/*none*/,/*none*/)
MAKE_ASSIGNMENT(Const, const)

#undef MAKE_ASSIGNMENT


}} //end namespace echo::k_array
