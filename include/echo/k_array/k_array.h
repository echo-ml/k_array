#pragma once

#include <echo/k_array/k_array_fwd.h>
#include <echo/k_array/k_array_accessor.h>
#include <echo/k_array/concept.h>
#include <echo/k_array/k_array_traits.h>
#include <echo/k_array/shaped.h>
#include <echo/k_array/shape_traits.h>
#include <echo/k_array/k_array_initializer.h>
#include <echo/memory.h>

#include <memory>
#include <algorithm>

namespace echo {
namespace k_array {

//------------------------------------------------------------------------------
// KArray
//------------------------------------------------------------------------------
template <class T, class Shape, class Allocator>
class KArray : htl::Pack<Shape>,
               htl::Pack<Allocator>,
               public KArrayAccessor<KArray<T, Shape, Allocator>, Shape>,
               public KArrayInitializer<KArray<T, Shape, Allocator>, T, Shape> {
  CONCEPT_ASSERT(concept::contiguous_shape<Shape>(),
                 "shape must be contiguous");

 public:
  using pointer = allocator_traits::pointer<Allocator>;
  using const_pointer = allocator_traits::const_pointer<Allocator>;
  using reference = iterator_traits::reference<pointer>;
  using const_reference = iterator_traits::reference<const_pointer>;
  using value_type = T;
  using memory_backend_tag =
      memory_backend_traits::memory_backend_tag<Allocator>;

  explicit KArray(const Shape& shape = Shape(),
                  const Allocator& allocator = Allocator())
      : htl::Pack<Shape>(shape), htl::Pack<Allocator>(allocator) {
    _data = nullptr;
    if (get_num_elements(shape) > 0)
      _data = this->allocator().allocate(get_num_elements(*this));
  }

  KArray(const KArray& other) : KArray(other.shape(), other.allocator()) {
    copy_construct(other);
  }

  template <class OtherT, class OtherAllocator,
            CONCEPT_REQUIRES(std::is_convertible<OtherT, T>::value)>
  KArray(const KArray<OtherT, Shape, OtherAllocator>& other) {
    copy_construct(other);
  }

  KArray(KArray&& other) : 
    KArray(Shape(), other.allocator())
  {
    swap(other);  
  }

  ~KArray() { release(); }

  KArray& operator=(const KArray& other) { return copy_assign(other); }

  template <class OtherT, class OtherAllocator,
            CONCEPT_REQUIRES(std::is_convertible<OtherT, T>::value)>
  KArray& operator=(const KArray<OtherT, Shape, OtherAllocator>& other) {
    return copy_assign(other);
  }

  KArray& operator=(KArray&& other) noexcept {
    swap(other);
    return *this;
  }

  KArray& operator=(
      InitializerMultilist<T, shape_traits::num_dimensions<Shape>()> values) {
    this->initialize(values);
    return *this;
  }

  void swap(KArray& other) noexcept {
    std::swap(this->modifiable_shape(), other.modifiable_shape());
    std::swap(_data, other._data);
  }

  pointer data() { return _data; }

  const_pointer data() const { return _data; }

  const_pointer const_data() const { return _data; }

  const Shape& shape() const { return htl::unpack<Shape>(*this); }

  const Allocator& allocator() const { return htl::unpack<Allocator>(*this); }
  Allocator& allocator() { return htl::unpack<Allocator>(*this); }

 private:
  Shape& modifiable_shape() { return htl::unpack<Shape>(*this); }
  template <class OtherT, class OtherAllocator,
            CONCEPT_REQUIRES(std::is_convertible<OtherT, T>::value)>
  void copy_construct(const KArray<OtherT, Shape, OtherAllocator>& other) {
    auto other_num_elements = get_num_elements(other);
    modifiable_shape() = other.shape();
    _data = this->allocator().allocate(other_num_elements);
    copy(memory_backend_traits::memory_backend_tag<OtherAllocator>(),
         other.data(), std::next(other.data(), other_num_elements),
         memory_backend_traits::memory_backend_tag<Allocator>(), _data);
  }

  template <class OtherT, class OtherAllocator,
            CONCEPT_REQUIRES(std::is_convertible<OtherT, T>::value)>
  KArray& copy_assign(const KArray<OtherT, Shape, OtherAllocator>& other) {
    if (this == std::addressof(other)) return *this;
    auto this_num_elements = get_num_elements(*this);
    auto other_num_elements = get_num_elements(other);
    // TODO: allow for reallocation
    if (this_num_elements != other_num_elements) {
      release();
      _data = this->allocator().allocate(other_num_elements);
    }
    modifiable_shape() = other.shape();
    copy(memory_backend_traits::memory_backend_tag<OtherAllocator>(),
         other.data(), std::next(other.data(), other_num_elements),
         memory_backend_traits::memory_backend_tag<Allocator>(), _data);
    return *this;
  }

  void release() noexcept {
    if (!_data) return;
    this->allocator().deallocate(_data, get_num_elements(*this));
    _data = nullptr;
  }
  pointer _data;
};

template <class T, class Shape, int Alignment>
class KArray<T, Shape, memory::StaticAllocator<T, Alignment>>
    : htl::Pack<Shape>,
      static_allocator_traits::buffer_type<
          memory::StaticAllocator<T, Alignment>,
          decltype(get_num_elements(Shape()))::value>,
      public KArrayAccessor<
          KArray<T, Shape, memory::StaticAllocator<T, Alignment>>, Shape>,
      public KArrayInitializer<
          KArray<T, Shape, memory::StaticAllocator<T, Alignment>>, T, Shape> {
  CONCEPT_ASSERT(concept::contiguous_shape<Shape>(),
                 "shape must be contiguous");
  CONCEPT_ASSERT(concept::static_shape<Shape>(),
                 "shape must be static for static arrays");

 public:
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using value_type = T;
  using memory_backend_tag = memory_backend_traits::memory_backend_tag<
      memory::StaticAllocator<T, Alignment>>;

  using Buffer = static_allocator_traits::buffer_type<
      memory::StaticAllocator<T, Alignment>,
      decltype(get_num_elements(Shape()))::value>;

  using Buffer::data;
  using Buffer::const_data;

  auto& operator=(
      InitializerMultilist<T, shape_traits::num_dimensions<Shape>()> values) {
    this->initialize(values);
    return *this;
  }

  void swap(KArray& other) { std::swap(*this, other); }

  const Shape& shape() const { return htl::unpack<Shape>(*this); }
};
}  // namespace k_array
}  // namespace echo
