#pragma once

#include <echo/k_array/k_array_accessor.h>
#include <echo/k_array/k_array_assignment.h>

#include <memory>
#include <algorithm>

namespace echo { namespace k_array {

template<
    class T
  , class Shape
  , class Allocator = std::allocator<T>
>
class KArray
  : Shape
  , public KArrayAccessor<
        KArray<T, Shape, Allocator>
      , typename std::allocator_traits<Allocator>::pointer
    >
  , public KArrayConstAccessor<
        KArray<T, Shape, Allocator>
      , typename std::allocator_traits<Allocator>::const_pointer
    >
  , public KArrayAssignment<
        KArray<T, Shape, Allocator>
      , T
    >
  , Allocator
{
  static_assert(is_contiguous_shape<Shape>(), "shape must be contiguous");
 public:
  using pointer         = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer   = typename std::allocator_traits<Allocator>::const_pointer;
  using reference       = typename std::iterator_traits<pointer>::reference;
  using const_reference = typename std::iterator_traits<const_pointer>::reference;
  using value_type      = T;

  using KArrayAccessor<KArray, pointer>::operator();
  using KArrayConstAccessor<KArray, const_pointer>::operator();
  using KArrayAssignment<KArray, T>::operator=;

  explicit KArray(const Shape& shape, Allocator allocator=Allocator()) 
    : Shape(shape)
    , Allocator(allocator)
  {
    _data = this->allocate(get_num_elements(*this));
  }

  KArray(const KArray& other)
    : Shape(other)
    , Allocator(other)
  {
    auto num_elements = get_num_elements(*this);
    _data = this->allocate(num_elements);

    //TODO: allow for customization point on copy
    std::copy_n(other._data, num_elements, _data);
  }

  KArray(KArray&& other)
    : Shape(other)
    , Allocator(other)
  {
    _data = other._data;
    other._data = nullptr;
  }

  ~KArray() {
    release();
  }

  KArray& operator=(const KArray& other) {
    if(this == std::addressof(other))
      return *this;

    auto this_num_elements  = get_num_elements(*this);
    auto other_num_elements = get_num_elements(other);

    //TODO: allow for reallocation
    if(this_num_elements != other_num_elements) {
      release();
      _data = this->allocate(other_num_elements);
    }

    static_cast<Shape&>(*this) = other.shape();

    //TODO: allow for customization point on copy
    std::copy_n(other._data, other_num_elements, _data);

    return *this;
  }

  KArray& operator=(KArray&& other) noexcept {
    if(this == std::addressof(other))
      return *this;
    
    release();
    
    static_cast<Shape&>(*this) = other.shape();

    _data = other._data;
    other._data = nullptr;

    return *this;
  }

  void swap(KArray& other) noexcept {
    std::swap(static_cast<Shape&>(*this), static_cast<Shape&>(other));
    std::swap(_data, other._data);
  }

  pointer data() {
    return _data;
  }

  const_pointer data() const {
    return _data;
  }

  const Shape& shape() const {
    return static_cast<const Shape&>(*this);
  }

 private:
  void release() noexcept {
    this->deallocate(_data, get_num_elements(*this));
    _data = nullptr;
  }
  pointer _data;
};

}} //end namespace echo::k_array
