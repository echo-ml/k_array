#pragma once

#include <echo/type_traits.h>
#include <echo/k_array/k_array_accessor.h>
#include <echo/k_array/k_array_assignment.h>

#include <iterator>

namespace echo {
namespace k_array {

template <class Pointer, class Shape>
class KArrayView
    : Shape,
      public KArrayConstAccessor<KArrayView<Pointer, Shape>, Shape> {
  CONCEPT_ASSERT(echo::concept::contiguous_iterator<Pointer>(),
                 "Pointer must be a contiguous iterator");
  CONCEPT_ASSERT(concept::shape<Shape>(), "Shape does not model Shape concept");

 public:
  using pointer = Pointer;
  using const_pointer =
      typename type_traits::ConvertToConstPointer<pointer>::type;
  using reference = typename std::iterator_traits<Pointer>::reference;
  using const_reference =
      typename std::iterator_traits<const_pointer>::reference;
  using value_type = typename std::iterator_traits<Pointer>::value_type;

  KArrayView(pointer data, const Shape& shape) noexcept : Shape(shape),
                                                          _data(data) {}

  pointer data() const { return _data; }

  const_pointer const_data() const { return _data; }

  const Shape& shape() const { return static_cast<const Shape&>(*this); }

 private:
  pointer _data;
};

template <class Pointer, class Shape,
          CONCEPT_REQUIRES(echo::concept::contiguous_iterator<Pointer>() &&
                           concept::shape<Shape>())>
auto make_k_array_view(Pointer data, const Shape& shape) {
  return KArrayView<Pointer, Shape>(data, shape);
}

template <class KArray,
          CONCEPT_REQUIRES(concept::contiguous_k_array<uncvref_t<KArray>>())>
auto make_k_array_view(KArray&& k_array) {
  return make_k_array_view(k_array.data(), k_array.shape());
}

////////////////////////
// make_k_array_cview //
////////////////////////

template <class KArray, CONCEPT_REQUIRES(concept::contiguous_k_array<KArray>())>
auto make_k_array_cview(const KArray& k_array) {
  return make_k_array_view(k_array.const_data(), k_array.shape());
}
}  // namespace k_array
}  // namespace echo
