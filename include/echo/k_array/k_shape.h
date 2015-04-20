#pragma once

#include <fatal/type/list.h>
#include <echo/index.h>
#include <echo/const_algorithm.h>
#include <echo/k_array/shape.h>
#include <echo/k_array/k_sequence.h>

namespace echo {
namespace k_array {

template <index_t...>
class KShape;

/////////////////////////////////////
// get_k_shape_from_dimension_list //
/////////////////////////////////////

namespace detail {
namespace k_shape {

template <index_t... Dimensions>
auto get_k_shape_from_dimension_list(
    fatal::type_list<std::integral_constant<index_t, Dimensions>...>)
    -> KShape<Dimensions...>;
}  // namespace k_shape
}  // namespace detail

//////////////////
// GetSubKShape //
//////////////////

namespace detail {
namespace k_shape {

template <class Shape>
struct GetSubKShape {
  using DimensionList = typename Shape::Dimensionality::list;
  using SubdimensionList =
      typename DimensionList::template left<DimensionList::size - 1>;
  using type = decltype(get_k_shape_from_dimension_list(SubdimensionList()));
};
}  // namespace k_shape
}  // namespace detail

////////////
// KShape //
////////////

namespace detail {
namespace k_shape {

struct k_shape_tag {};
}  // namespace k_shape
}  // namespace detail

template <index_t... Dimensions>
class KShape : KSequence<detail::k_shape::k_shape_tag,
                         const_algorithm::count(StaticIndex<Dimensions...>(),
                                                Dimension::Dynamic())> {
 public:
  static constexpr index_t kNumDynamicExtents = const_algorithm::count(
      StaticIndex<Dimensions...>(), Dimension::Dynamic());
  using Base = KSequence<detail::k_shape::k_shape_tag, kNumDynamicExtents>;

  using Dimensionality = StaticIndex<Dimensions...>;
  using Subshape = typename detail::k_shape::GetSubKShape<KShape>::type;

  KShape() {}

  template <class... Extents,
            CONCEPT_REQUIRES(const_algorithm::and_(fatal::constant_sequence<
                bool, std::is_convertible<Extents, index_t>::value...>()))>
  explicit KShape(Extents... dynamic_extents)
      : Base(dynamic_extents...) {}

  explicit KShape(const Index<kNumDynamicExtents>& extents) : Base(extents) {}

  template <int I>
  index_t dynamic_extent() const {
    return Base::template extent<I>();
  }

  const Subshape& subshape() const {
    return reinterpret_cast<const Subshape&>(*this);
  }

  const auto& dynamic_extents() const { return Base::extents(); }
};

///////////////////
// GetKShapeType //
///////////////////

namespace detail {
namespace k_shape {

inline fatal::constant_sequence<index_t> get_dimensionality_sequence() {
  return {};
}

template <class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, index_t>::value &&
                           ExtentFirst::value > 0)>
auto get_dimensionality_sequence(ExtentFirst extent_first,
                                 ExtentsRest... extents_rest);

template <class ExtentFirst, class... ExtentsRest,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, index_t>::value)>
auto get_dimensionality_sequence(ExtentFirst extent_first,
                                 ExtentsRest... extents_rest) {
  using DimensionalityRest =
      decltype(get_dimensionality_sequence(extents_rest...));
  return
      typename DimensionalityRest::template push_front<Dimension::kDynamic>();
}

template <
    class ExtentFirst, class... ExtentsRest,
    CONCEPT_REQUIRES_REDECLARATION(!std::is_same<ExtentFirst, index_t>::value &&
                                   ExtentFirst::value > 0)>
auto get_dimensionality_sequence(ExtentFirst extent_first,
                                 ExtentsRest... extents_rest) {
  using DimensionalityRest =
      decltype(get_dimensionality_sequence(extents_rest...));
  return typename DimensionalityRest::template push_front<ExtentFirst::value>();
}

template <index_t... Extents>
auto get_k_shape_type(fatal::constant_sequence<index_t, Extents...>)
    -> KShape<Extents...>;

template <class... Extents>
using GetKShapeType = decltype(
    get_k_shape_type(get_dimensionality_sequence(std::declval<Extents>()...)));
}  // namespace k_shape
}  // end namespace detail

/////////////////////////
// set_dynamic_extents //
/////////////////////////

namespace detail {
namespace k_shape {

template <int I, int N>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents) {}

template <int I, class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(std::is_same<ExtentFirst, index_t>::value)>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents,
                                 ExtentFirst extent_first,
                                 ExtentsRest... extents_rest);

template <int I, class ExtentFirst, class... ExtentsRest, int N,
          CONCEPT_REQUIRES(!std::is_same<ExtentFirst, index_t>::value)>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents,
                                 ExtentFirst extent_first,
                                 ExtentsRest... extents_rest) {
  set_dynamic_k_shape_extents<I>(dynamic_extents, extents_rest...);
}

template <
    int I, class ExtentFirst, class... ExtentsRest, int N,
    CONCEPT_REQUIRES_REDECLARATION(std::is_same<ExtentFirst, index_t>::value)>
void set_dynamic_k_shape_extents(Index<N>& dynamic_extents,
                                 ExtentFirst extent_first,
                                 ExtentsRest... extents_rest) {
  dynamic_extents[I] = extent_first;
  set_dynamic_k_shape_extents<I + 1>(dynamic_extents, extents_rest...);
}
}  // namespace k_shape
}  // namespace detail

//////////////////
// make_k_shape //
//////////////////

template <class... Extents,
          CONCEPT_REQUIRES(const_algorithm::and_(
              fatal::constant_sequence<bool, concept::extent<Extents>()...>()))>
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

template <class... Extents>
using KShapeFromExtents = decltype(make_k_shape(std::declval<Extents>()...));
}  // namespace k_array
}  // namespace echo
