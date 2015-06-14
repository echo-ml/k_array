#pragma once

#include <echo/k_array2/shape_traits.h>
#include <echo/utility/repeat_type.h>
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
  decltype(auto) operator()(repeat_type_c<Indexes, index_t>... indexes) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    return *(derived.data() + get_1d_index(derived.shape(), indexes...));
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
  decltype(auto) operator()(repeat_type_c<Indexes, index_t>... indexes) {
    Derived& derived = static_cast<Derived&>(*this);
    return const_cast<decltype(*derived.data())>(
        const_cast<const KArrayAccessorImpl&>(*this).operator()(indexes...));
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
