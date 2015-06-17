#pragma once

#include <echo/k_array2/concept.h>

namespace echo { namespace k_array {

namespace slice {
  
/////////
// all //
/////////

struct all_t {};
constexpr auto all_t all{};

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

//////////////////
// CountedRange //
//////////////////

template<class Extent>
class CountedRange {
 public:
  
 private:
  index_t i;
  Extent _extent;
};

}

namespace echo { namespace slice {



template<class Shape, class Splices>
auto get_subdimensions(const Shape& shape, const Slices& slices) {
  struct empty_dimension {};
  auto get_dimension = [&](auto index) {
    using Index = decltype(index);
  };
}

}}

///////////////////
// make_subshape //
///////////////////

template<class Shape, class... Slices>
auto make_subshape(const Shape& shape, Slices...) {
}

}}
