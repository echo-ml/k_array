#pragma once

#include <echo/k_array/k_array_view_fwd.h>
#include <echo/type_traits.h>
#include <echo/k_array/k_array_accessor.h>
#include <echo/k_array/k_array_initializer.h>

#include <iterator>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// KArrayView
//------------------------------------------------------------------------------
template <class Pointer, class Shape, class MemoryBackendTag>
class KArrayView
    : htl::Pack<Shape>,
      public KArrayConstAccessor<KArrayView<Pointer, Shape>, Shape>,
      public KArrayConstInitializer<KArrayView<Pointer, Shape>,
                                    iterator_traits::value_type<Pointer>,
                                    Shape> {
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
  using memory_backend_tag = MemoryBackendTag;

  KArrayView(pointer data, const Shape& shape) noexcept
      : htl::Pack<Shape>(shape),
        _data(data) {}

  pointer data() const { return _data; }

  const_pointer const_data() const { return _data; }

  const Shape& shape() const { return htl::unpack<Shape>(*this); }

  const auto& operator=(
      InitializerMultilist<iterator_traits::value_type<Pointer>,
                           shape_traits::num_dimensions<Shape>()> values)
      const {
    this->initialize(values);
    return *this;
  }

 private:
  pointer _data;
};

//------------------------------------------------------------------------------
// make_k_array_view
//------------------------------------------------------------------------------
template <class Pointer, class Shape,
          CONCEPT_REQUIRES(echo::concept::contiguous_iterator<Pointer>() &&
                           concept::shape<Shape>())>
auto make_k_array_view(Pointer data, const Shape& shape) {
  return KArrayView<Pointer, Shape>(data, shape);
}

template <class MemoryBackendTag, class Pointer, class Shape,
          CONCEPT_REQUIRES(echo::concept::contiguous_iterator<Pointer>() &&
                           concept::shape<Shape>())>
auto make_k_array_view(Pointer data, const Shape& shape) {
  return KArrayView<Pointer, Shape, MemoryBackendTag>(data, shape);
}

//------------------------------------------------------------------------------
// make_view
//------------------------------------------------------------------------------
template <class KArray, CONCEPT_REQUIRES(concept::k_array<uncvref_t<KArray>>())>
auto make_view(KArray&& k_array) {
  return make_k_array_view<typename uncvref_t<KArray>::memory_backend_tag>(
      k_array.data(), k_array.shape());
}

//------------------------------------------------------------------------------
// make_cview
//------------------------------------------------------------------------------
template <class KArray, CONCEPT_REQUIRES(concept::k_array<uncvref_t<KArray>>())>
auto make_cview(KArray&& k_array) {
  return make_k_array_view<typename uncvref_t<KArray>::memory_backend_tag>(
      k_array.const_data(), k_array.shape());
}

}  // namespace k_array
}  // namespace echo
