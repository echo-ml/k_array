#pragma once

#include <echo/k_array/shape.h>
#include <iterator>

namespace echo {
namespace k_array {

template <class Derived, class Pointer>
struct KArrayConstAccessor {
  template <class... Indexes>
  decltype(auto) operator()(Indexes... indexes) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    return *(derived.data() + get_1d_index(derived.shape(), indexes...));
  }
  decltype(auto) operator[](echo::Index<1> index) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    return *(derived.data() + index);
  }
};

template <class Derived, class Pointer>
struct KArrayAccessor : KArrayConstAccessor<Derived, Pointer> {
  using KArrayConstAccessor<Derived, Pointer>::operator();
  using KArrayConstAccessor<Derived, Pointer>::operator[];
  template <class... Indexes>
  decltype(auto) operator()(Indexes... indexes) {
    Derived& derived = static_cast<Derived&>(*this);
    return *(derived.data() + get_1d_index(derived.shape(), indexes...));
  }
  decltype(auto) operator[](echo::Index<1> index) {
    Derived& derived = static_cast<Derived&>(*this);
    return *(derived.data() + index);
  }
};
}
}  // end namespace echo::k_array
