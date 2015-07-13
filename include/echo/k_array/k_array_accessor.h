#pragma once

#define DETAIL_NS detail_k_array_accessor

#include <echo/k_array/shape_traits.h>
#include <echo/repeat_type.h>
#include <echo/access_mode.h>
#include <iterator>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// KArrayConstAccessor
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Indexes, class Derived, class Shape>
struct KArrayConstAccessorImpl {};

template <std::size_t... Indexes, class Derived, class Shape>
struct KArrayConstAccessorImpl<std::index_sequence<Indexes...>, Derived,
                               Shape> {
  decltype(auto) operator()(access_mode::readonly_t,
                            repeat_type_c<Indexes, index_t>... indexes) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    CONTRACT_EXPECT {
      CONTRACT_ASSERT(within_dimensions(derived, {indexes...}));
    };
    return *(derived.const_data() + get_1d_index(derived.shape(), indexes...));
  }
  decltype(auto) operator()(repeat_type_c<Indexes, index_t>... indexes) const {
    const Derived& derived = static_cast<const Derived&>(*this);
    CONTRACT_EXPECT {
      CONTRACT_ASSERT(within_dimensions(derived, {indexes...}));
    };
    return *(derived.data() + get_1d_index(derived.shape(), indexes...));
  }
};
}

template <class Derived, class Shape>
struct KArrayConstAccessor
    : DETAIL_NS::KArrayConstAccessorImpl<
          std::make_index_sequence<shape_traits::num_dimensions<Shape>()>,
          Derived, Shape> {};

//------------------------------------------------------------------------------
// KArrayAccessor
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class Indexes, class Derived, class Shape>
struct KArrayAccessorImpl {};

template <std::size_t... Indexes, class Derived, class Shape>
struct KArrayAccessorImpl<std::index_sequence<Indexes...>, Derived, Shape>
    : KArrayConstAccessor<Derived, Shape> {
  using KArrayConstAccessor<Derived, Shape>::operator();
  decltype(auto) operator()(access_mode::readonly_t,
                            repeat_type_c<Indexes, index_t>... indexes) {
    Derived& derived = static_cast<Derived&>(*this);
    CONTRACT_EXPECT {
      CONTRACT_ASSERT(within_dimensions(derived, {indexes...}));
    };
    return const_cast<decltype(*derived.data())>(
        const_cast<const KArrayAccessorImpl&>(*this).operator()(
            access_mode::readonly, indexes...));
  }
  decltype(auto) operator()(repeat_type_c<Indexes, index_t>... indexes) {
    Derived& derived = static_cast<Derived&>(*this);
    CONTRACT_EXPECT {
      CONTRACT_ASSERT(within_dimensions(derived, {indexes...}));
    };
    return const_cast<decltype(*derived.data())>(
        const_cast<const KArrayAccessorImpl&>(*this).operator()(indexes...));
  }
};
}

template <class Derived, class Shape>
struct KArrayAccessor
    : DETAIL_NS::KArrayAccessorImpl<
          std::make_index_sequence<shape_traits::num_dimensions<Shape>()>,
          Derived, Shape> {};

}  // namespace k_array
}  // namespace echo

#undef DETAIL_NS
