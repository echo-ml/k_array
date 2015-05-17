#pragma once

#include <echo/k_array/shape.h>
#include <iterator>

namespace echo {
namespace k_array {

/////////////////////////
// KArrayConstAccessor //
/////////////////////////

namespace detail {
namespace k_array_accessor {

template <class Indexes, class Derived, class Shape>
struct KArrayConstAccessorImpl {};

template <std::size_t... Indexes, class Derived, class Shape>
struct KArrayConstAccessorImpl<std::index_sequence<Indexes...>, Derived,
                               Shape> {
  decltype(auto) operator()(
      std::enable_if_t<true || Indexes, index_t>... indexes) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    return *(derived.data() + get_1d_index(derived.shape(), indexes...));
  }
  decltype(auto) operator[](echo::Index<1> index) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    return *(derived.data() + index);
  }
};
}
}

template <class Derived, class Shape>
struct KArrayConstAccessor
    : detail::k_array_accessor::KArrayConstAccessorImpl<
          std::make_index_sequence<shape_traits::num_dimensions<Shape>()>,
          Derived, Shape> {};

////////////////////
// KArrayAccessor //
////////////////////

namespace detail {
namespace k_array_accessor {

template <class Indexes, class Derived, class Shape>
struct KArrayAccessorImpl {};

template <std::size_t... Indexes, class Derived, class Shape>
struct KArrayAccessorImpl<std::index_sequence<Indexes...>, Derived, Shape>
    : KArrayConstAccessor<Derived, Shape> {
  using KArrayConstAccessor<Derived, Shape>::operator();
  using KArrayConstAccessor<Derived, Shape>::operator[];
  decltype(auto) operator()(
      std::enable_if_t<true || Indexes, index_t>... indexes) {
    Derived& derived = static_cast<Derived&>(*this);
    return const_cast<decltype(*derived.data())>(
        const_cast<const KArrayAccessorImpl&>(*this).operator()(indexes...));
  }
  decltype(auto) operator[](echo::Index<1> index) {
    Derived& derived = static_cast<Derived&>(*this);
    return const_cast<decltype(*derived.data())>(
        const_cast<const KArrayAccessorImpl&>(*this).operator[](index));
  }
};
}
}

template <class Derived, class Shape>
struct KArrayAccessor
    : detail::k_array_accessor::KArrayAccessorImpl<
          std::make_index_sequence<shape_traits::num_dimensions<Shape>()>,
          Derived, Shape> {};

}  // namespace k_array
}  // namespace echo
