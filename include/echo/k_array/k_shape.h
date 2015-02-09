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

namespace detail { namespace k_shape {

template<class Shape>
struct GetSubKShape {
  using DimensionList    = typename Shape::Dimensionality::list;
  using SubdimensionList = typename DimensionList::template left<DimensionList::size-1>;
  using type = decltype(get_k_shape_from_dimension_list(SubdimensionList()));
};

}} //end namespace detail::k_shape

////////////
// KShape //
////////////

namespace detail { namespace k_shape {

struct k_shape_tag {};

}} //end namespace detail::k_shape

template<IndexInteger... Dimensions>
class KShape 
  : KSequence<
        detail::k_shape::k_shape_tag
      , const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic())
  >
{
 public:
  static constexpr IndexInteger kNumDynamicExtents = 
              const_algorithm::count(StaticIndex<Dimensions...>(), Dimension::Dynamic());
  using Base = KSequence<detail::k_shape::k_shape_tag, kNumDynamicExtents>;

  using Dimensionality = StaticIndex<Dimensions...>;
  using Subshape       = typename detail::k_shape::GetSubKShape<KShape>::type;

  KShape() {}

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

///////////////////
// GetKShapeType //
///////////////////

namespace detail { namespace k_shape {

inline
fatal::constant_sequence<IndexInteger> get_dimensionality_sequence() 
{
  return {};
}

template<
    class ExtentFirst
  , class... ExtentsRest
  , disable_if<std::is_same<ExtentFirst, IndexInteger>> = 0
  , enable_if_c<(ExtentFirst::value > 0)> = 0
>
auto get_dimensionality_sequence(ExtentFirst extent_first, ExtentsRest... extents_rest);

template<
    class ExtentFirst
  , class... ExtentsRest
  , enable_if<std::is_same<ExtentFirst, IndexInteger>> = 0
>
auto get_dimensionality_sequence(ExtentFirst extent_first, ExtentsRest... extents_rest) {
  using DimensionalityRest = decltype(get_dimensionality_sequence(extents_rest...));
  return typename DimensionalityRest::template push_front<Dimension::kDynamic>();
}

template<
    class ExtentFirst
  , class... ExtentsRest
  , disable_if<std::is_same<ExtentFirst, IndexInteger>>
  , enable_if_c<(ExtentFirst::value > 0)>
>
auto get_dimensionality_sequence(ExtentFirst extent_first, ExtentsRest... extents_rest) {
  using DimensionalityRest = decltype(get_dimensionality_sequence(extents_rest...));
  return typename DimensionalityRest::template push_front<ExtentFirst::value>();
}

template<
    IndexInteger... Extents
>
auto get_k_shape_type(fatal::constant_sequence<IndexInteger, Extents...>) {
  return std::declval<KShape<Extents...>>();
}

template<class... Extents>
using GetKShapeType = decltype(
    get_k_shape_type(get_dimensionality_sequence(std::declval<Extents>()...))
);

}} //end namespace detail

/////////////////////////
// set_dynamic_extents //
/////////////////////////

namespace detail { namespace k_shape {

template<int I, int N>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents) {}
  

template<
    int I
  , class ExtentFirst
  , class... ExtentsRest
  , int N
  , enable_if<std::is_same<ExtentFirst, IndexInteger>> = 0
>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents
                               , ExtentFirst extent_first
                               , ExtentsRest... extents_rest);

template<
    int I
  , class ExtentFirst
  , class... ExtentsRest
  , int N
  , disable_if<std::is_same<ExtentFirst, IndexInteger>> = 0
>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents
                               , ExtentFirst extent_first
                               , ExtentsRest... extents_rest)
{
  set_dynamic_k_shape_extents<I>(dynamic_extents, extents_rest...);
}

template<
    int I
  , class ExtentFirst
  , class... ExtentsRest
  , int N
  , enable_if<std::is_same<ExtentFirst, IndexInteger>>
>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents
                               , ExtentFirst extent_first
                               , ExtentsRest... extents_rest)
{
  dynamic_extents[I] = extent_first;
  set_dynamic_k_shape_extents<I+1>(dynamic_extents, extents_rest...);
}

}} //end namespace detail::k_shape

//////////////////
// make_k_shape //
//////////////////

template<class... Extents>
auto make_k_shape(Extents... extents) {
  using namespace detail::k_shape;
  using Result = GetKShapeType<Extents...>;
  Index<Result::kNumDynamicExtents> dynamic_extents;
  set_dynamic_k_shape_extents<0>(dynamic_extents, extents...);
  return Result(dynamic_extents);
}

///////////////////////
// KShapeFromExtents //
///////////////////////

template<class... Extents>
using KShapeFromExtents = decltype(make_k_shape(std::declval<Extents>()...));

}} //end namespace
