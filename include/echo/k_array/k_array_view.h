#pragma once

#include <echo/k_array/k_array_accessor.h>
#include <echo/k_array/k_array_assignment.h>
#include <echo/k_array/k_array_concept.h>

#include <iterator>

namespace echo { namespace k_array {

template<class Pointer, class Shape>
class KArrayView 
  : public Shape
  , public KArrayConstAccessor<
        KArrayView<Pointer, Shape>
      , Pointer
    >
{
 public:
  using pointer    = Pointer;
  using reference  = typename std::iterator_traits<Pointer>::reference;
  using value_type = typename std::iterator_traits<Pointer>::value_type;

  KArrayView(pointer data, const Shape& shape) noexcept 
    : Shape(shape)
    , _data(data) 
  {}
  
  pointer data() const { return _data; }

  const Shape& shape() const {
    return static_cast<const Shape&>(*this);
  }
 private:
  pointer _data;
};
    

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
>
auto make_k_array_view(KArray&& k_array) {
  using Pointer = decltype(k_array.data());
  using Shape   = decltype(k_array.shape());
  return KArrayView<
      k_array_traits::Pointer<KArray>
    , k_array_traits::Shape<KArray>
  >(k_array.data(), k_array.shape());
}

template<
    class KArray
  , enable_if<is_k_array<KArray>> = 0
>
auto make_k_array_const_view(const KArray& k_array) {
  return make_k_array_view(k_array);
}

}} //end namespace
