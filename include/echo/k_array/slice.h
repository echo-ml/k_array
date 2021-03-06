#pragma once

#define DETAIL_NS detail_slice

#include <echo/k_array/concept.h>
#include <echo/contract.h>

namespace echo {
namespace k_array {

namespace slice {

//------------------------------------------------------------------------------
// all
//------------------------------------------------------------------------------
struct all_t {};
constexpr all_t all{};

namespace concept {
template <class T>
constexpr bool all() {
  return std::is_same<T, all_t>::value;
}
}

//------------------------------------------------------------------------------
// Range
//------------------------------------------------------------------------------
class Range {
 public:
  Range(index_t a, index_t b) : _a(a), _b(b) {
    CONTRACT_EXPECT {
      CONTRACT_ASSERT(a >= 0);
      CONTRACT_ASSERT(a <= b);
    };
  }
  auto a() const { return _a; }
  auto b() const { return _b; }

 private:
  index_t _a, _b;
};

inline auto range(index_t a, index_t b) { return Range(a, b); }

namespace concept {
template <class T>
constexpr bool range() {
  return std::is_same<T, Range>::value;
}
}

//------------------------------------------------------------------------------
// CountedRange
//------------------------------------------------------------------------------
template <class Extent>
class CountedRange {
 public:
  CountedRange(index_t a, Extent extent) : _a(a), _extent(extent) {
    CONTRACT_EXPECT {
      CONTRACT_ASSERT(a >= 0);
      CONTRACT_ASSERT(extent >= 0);
    };
  }
  index_t a() const { return _a; }
  Extent extent() const { return _extent; }

 private:
  index_t _a;
  Extent _extent;
};

template <class Extent>
auto counted_range(index_t a, Extent extent) {
  return CountedRange<Extent>(a, extent);
}

namespace concept {
namespace DETAIL_NS {
template <class Extent>
auto counted_range_impl(CountedRange<Extent> && ) -> std::true_type;

template <class T>
auto counted_range_impl(T && ) -> std::false_type;
}

template <class T>
constexpr bool counted_range() {
  using Result = decltype(DETAIL_NS::counted_range_impl(std::declval<T>()));
  return Result::value;
}
}

//------------------------------------------------------------------------------
// get_first_index
//------------------------------------------------------------------------------
inline index_t get_first_index(index_t a) { return a; }

inline index_t get_first_index(all_t) { return 0; }

inline index_t get_first_index(const Range& range) { return range.a(); }

template <class Extent>
index_t get_first_index(const CountedRange<Extent>& range) {
  return range.a();
}
}

namespace concept {

//------------------------------------------------------------------------------
// slice
//------------------------------------------------------------------------------
template <class T>
constexpr bool slice() {
  return slice::concept::all<T>() || slice::concept::range<T>() ||
         slice::concept::counted_range<T>() ||
         std::is_convertible<T, index_t>();
}

//------------------------------------------------------------------------------
// proper_slices
//------------------------------------------------------------------------------
namespace DETAIL_NS {
template <class... Slices>
auto proper_slices_impl(Slices...) {
  return std::integral_constant<
      bool, and_c<std::is_convertible<Slices, index_t>::value...>()>();
}

template <class... SlicesRest>
auto proper_slices_impl(k_array::slice::all_t, SlicesRest... slices_rest) {
  return proper_slices_impl(slices_rest...);
}
}

template <class... Slices>
constexpr bool proper_slices() {
  using Result =
      decltype(DETAIL_NS::proper_slices_impl(std::declval<Slices>()...));
  return Result::value;
}
}
}
}

#undef DETAIL_NS
