#pragma once

#include <fatal/type/list.h>
#include <echo/enable_if.h>
#include <echo/index.h>
#include <echo/const_algorithm.h>
#include <echo/k_array/shape.h>

namespace echo { namespace k_array {

////////////////
// KShapeBase //
////////////////

namespace detail {

template<int N>
class KShapeBase {
 public:
  template<
      class... Extents
  >
  KShapeBase(Extents... extents) 
    : _extents(extents...)
  {
  }

  template<int I>
  IndexInteger extent() const {
    return get<I>(_extents);
  }
 private:
  Index<N> _extents;
};

template<>
class KShapeBase<0> {
};

} //end namespace detail

////////////
// KShape //
////////////

template<IndexInteger... Dimensions>
class KShape 
  : detail::KShapeBase<
        const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic())
  >
{
  static constexpr IndexInteger kNumDynamicExtents = 
              const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic());

  using Base = detail::KShapeBase<kNumDynamicExtents>;

 public:
  using Dimensionality = StaticIndex<Dimensions...>;

  template<
      class... Extents
    , enable_if<std::is_convertible<Extents, IndexInteger>...> = 0
  >
  KShape(Extents... dynamic_extents)
    : Base(dynamic_extents...)
  {}

  template<int I>
  IndexInteger dynamic_extent() const {
    return Base::template extent<I>();
  }
};

///////////////
// SubKShape //
///////////////

namespace detail {

template<IndexInteger... Dimensions>
auto get_k_shape_from_dimension_list(
        fatal::type_list<std::integral_constant<IndexInteger, Dimensions>...>)
{
  return std::declval<KShape<Dimensions...>>();
}

template<class Shape>
struct GetSubKShape {
  using DimensionList    = typename Shape::Dimensionality::list;
  using SubdimensionList = typename DimensionList::template left<DimensionList::size-1>;
  using type = decltype(get_k_shape_from_dimension_list(SubdimensionList()));
};

} //end namespace detail

template<class Shape>
using GetSubKShape = typename detail::GetSubKShape<Shape>::type;

template<IndexInteger... Dimensions>
const auto& get_sub_k_shape(const KShape<Dimensions...>& shape) {
  using Shape = KShape<Dimensions...>;
  return reinterpret_cast<const GetSubKShape<Shape>&>(shape);
}

}} //end namespace
