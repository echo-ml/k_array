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
  : public Shape
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
 public:
  using pointer       = typename std::allocator_traits<Allocator>::pointer;
  using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;

  using KArrayAccessor<KArray, pointer>::operator();
  using KArrayConstAccessor<KArray, const_pointer>::operator();
  using KArrayAssignment<KArray, T>::operator=;

  KArray(const Shape& shape, Allocator allocator=Allocator()) 
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

    static_cast<Shape&>(*this) = other;

    //TODO: allow for customization point on copy
    std::copy_n(other._data, other_num_elements, _data);

    return *this;
  }

  KArray& operator=(KArray&& other) noexcept {
    if(this == std::addressof(other))
      return *this;
    
    release();
    
    static_cast<Shape&>(*this) = other;

    _data = other._data;
    other._data = nullptr;
  }

  pointer data() {
    return _data;
  }

  const_pointer data() const {
    return _data;
  }
 private:
  void release() noexcept {
    this->deallocate(_data, get_num_elements(*this));
    _data = nullptr;
  }
  pointer _data;
};

}} //end namespace echo::k_array
