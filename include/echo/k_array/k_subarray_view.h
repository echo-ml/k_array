#pragma once

#include <echo/k_array/k_array_accessor.h>
#include <echo/k_array/k_array_assignment.h>
#include <echo/k_array/k_array_concept.h>

#include <iterator>

namespace echo {
namespace k_array {

template <class Pointer, class Shape>
class KSubarrayView
    : public Shape,
      public KArrayConstAccessor<KSubarrayView<Pointer, Shape>, Pointer> {
 public:
  using pointer = Pointer;
  using reference = typename std::iterator_traits<Pointer>::reference;
  using value_type = typename std::iterator_traits<Pointer>::value_type;

  KSubarrayView(pointer data, const Shape& shape) noexcept : Shape(shape),
                                                             _data(data) {}

  pointer data() const { return _data; }

  const Shape& shape() const { return static_cast<const Shape&>(*this); }

 private:
  pointer _data;
};
}  // namespace k_array
}  // namespace echo
