#pragma once

#include <echo/k_array2/concept.h>

namespace echo {
namespace k_array {

namespace slice {

/////////
// all //
/////////

struct all_t {};
constexpr all_t all{};

namespace concept {
template <class T>
constexpr bool all() {
  return std::is_same<T, all_t>::value;
}
}

///////////
// Range //
///////////

class Range {
 public:
  Range(index_t a, index_t b) : _a(a), _b(b) {}
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

//////////////////
// CountedRange //
//////////////////

template <class Extent>
class CountedRange {
 public:
  CountedRange(index_t a, Extent extent) : _a(a), _extent(extent) {}
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
namespace detail {
namespace slice {
template <class Extent>
auto counted_range_impl(CountedRange<Extent> && ) -> std::true_type;

template <class T>
auto counted_range_impl(T && ) -> std::false_type;
}
}

template <class T>
constexpr bool counted_range() {
  using Result = decltype(detail::slice::counted_range_impl(std::declval<T>()));
  return Result::value;
}
}
}

namespace concept {

///////////
// slice //
///////////

template <class T>
constexpr bool slice() {
  return slice::concept::all<T>() || slice::concept::range<T>() ||
         slice::concept::counted_range<T>() ||
         std::is_convertible<T, index_t>();
}

///////////////////
// proper_slices //
///////////////////

namespace detail {
namespace slice {

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
}

template <class... Slices>
constexpr bool proper_slices() {
  using Result =
      decltype(detail::slice::proper_slices_impl(std::declval<Slices>()...));
  return Result::value;
}
}
}
}
