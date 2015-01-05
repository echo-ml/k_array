#pragma once

#include <fatal/type/list.h>
#include <echo/enable_if.h>
#include <echo/index.h>
#include <echo/const_algorithm.h>
#include <echo/k_array/shape.h>
#include <echo/k_array/k_sequence.h>

namespace echo { namespace k_array {

template<IndexInteger...> class KShape;

/////////////////////////////////////
// get_k_shape_from_dimension_list //
/////////////////////////////////////

namespace detail {

template<IndexInteger... Dimensions>
auto get_k_shape_from_dimension_list(
        fatal::type_list<std::integral_constant<IndexInteger, Dimensions>...>)
    -> KShape<Dimensions...>;
}

//////////////////
// GetSubKShape //
//////////////////

namespace detail {

template<class Shape>
struct GetSubKShape {
  using DimensionList    = typename Shape::Dimensionality::list;
  using SubdimensionList = typename DimensionList::template left<DimensionList::size-1>;
  using type = decltype(get_k_shape_from_dimension_list(SubdimensionList()));
};

} //end namespace detail

//////////////////
// swap_k_shape //
//////////////////

namespace detail {

template<int I, int J, class Shape>
auto swap_k_shape(const Shape& shape) {
  auto extents = shape.extents();
  
}

} //end namespace detail

////////////
// KShape //
////////////

namespace detail {

struct k_shape_tag {};

} //end namespace detail

template<IndexInteger... Dimensions>
class KShape 
  : KSequence<
        detail::k_shape_tag
      , const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic())
  >
{
  static constexpr IndexInteger kNumDynamicExtents = 
              const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic());

  using Base = KSequence<detail::k_shape_tag, kNumDynamicExtents>;

 public:
  using Dimensionality = StaticIndex<Dimensions...>;
  using Subshape       = typename detail::GetSubKShape<KShape>::type;

  template<
      class... Extents
    , enable_if<std::is_convertible<Extents, IndexInteger>...> = 0
  >
  explicit KShape(Extents... dynamic_extents)
    : Base(dynamic_extents...)
  {}

  explicit KShape(const Index<kNumDynamicExtents>& extents) 
    : Base(extents)
  {
  }

  template<int I>
  IndexInteger dynamic_extent() const {
    return Base::template extent<I>();
  }

  const Subshape& subshape() const {
    return reinterpret_cast<const Subshape&>(*this);
  }

  const auto& extents() const {
    return Base::extents();
  }
};

}} //end namespace
