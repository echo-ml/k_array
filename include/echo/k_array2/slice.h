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
}

namespace detail {
namespace slice {

struct empty_dimension {};

template <class Extent>
auto get_subdimension(Extent extent, k_array::slice::all_t) {
  return extent;
}

template <class Extent>
auto get_subdimension(Extent, const k_array::slice::Range& range) {
  return range.b() - range.a();
}

template <class Extent>
auto get_subdimension(Extent, index_t) {
  return empty_dimension();
}

template <class Extent1, class Extent2>
auto get_subdimension(
    Extent1, const k_array::slice::CountedRange<Extent2>& counted_range) {
  return counted_range.extent();
}

template <class Shape, class Slices>
auto get_subdimensions(const Shape& shape, const Slices& slices) {
  auto subdimensions = htl::map([](auto extent, auto slice) {
    return get_subdimension(extent, slice);
  }, shape.extents(), slices);
  return make_dimensionality(htl::remove_if([](auto dimension) {
    return htl::integral_constant<
        bool, std::is_same<decltype(dimension), empty_dimension>::value>();
  }, subdimensions));
}

template<class Shape, class Slices>
auto get_subdimension_indexes_impl(const Slices& slices) {
  auto range =
      htl::make_index_sequence<htl::tuple_traits::num_elements<Slices>()>();

}

}
}

///////////////////
// make_subshape //
///////////////////

template <class Shape, class... Slices>
auto make_subshape(const Shape& shape, Slices...) {}
}
}
