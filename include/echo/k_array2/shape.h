#pragma once

#include <echo/k_array2/dimensionality.h>
#include <echo/k_array2/concept.h>
#include <echo/htl.h>

namespace echo {
namespace k_array {

///////////
// Shape //
///////////

template <class... Extents>
class Shape : public Dimensionality<Extents...> {
  using Base = Dimensionality<Extents...>;

 public:
  Shape() = default;
  explicit Shape(const Base& dimensionality) : Base(dimensionality) {}

  explicit Shape(Extents... extents) : Base(extents...) {}

  const auto& dimensionality() const { return static_cast<const Base&>(*this); }
};

////////////
// ShapeC //
////////////

template <index_t... ExtentsC>
using ShapeC = Shape<ExtentC<ExtentsC>...>;

////////////////
// make_shape //
////////////////

template <class... Extents>
auto make_shape(const Dimensionality<Extents...>& dimensionality) {
  return Shape<Extents...>(dimensionality);
}

template <class... Extents,
          CONCEPT_REQUIRES(and_c<concept::extent<Extents>()...>())>
auto make_shape(Extents... extents) {
  return Shape<Extents...>(extents...);
}

template <class... Extents,
          CONCEPT_REQUIRES(and_c<concept::extent<Extents>()...>())>
auto make_shape(const htl::Tuple<Extents...>& extents) {
  return reinterpret_cast<const Shape<Extents...>&>(extents);
}

////////////////
// get_stride //
////////////////

template <int I, class... Extents,
          CONCEPT_REQUIRES(I >= 0 && I < sizeof...(Extents))>
auto get_stride(const Shape<Extents...>& shape) {
  using Extents_ = uncvref_t<decltype(
      htl::left<I + 1>(htl::Tuple<StaticIndex<1>, Extents...>()))>;
  decltype(auto) extents_ = reinterpret_cast<const Extents_&>(shape.extents());
  return htl::left_fold([](auto x, auto y) { return x * y; }, 1_index,
                        extents_);
}

//////////////////
// get_1d_index //
//////////////////

namespace detail {
namespace shape {

template <class Extent>
index_t get_1d_index_impl(const htl::Tuple<Extent>&, index_t index) {
  return index;
}

template <class ExtentFirst, class... ExtentsRest, class... IndexesRest>
index_t get_1d_index_impl(
    const htl::Tuple<ExtentFirst, ExtentsRest...>& extents, index_t index_first,
    IndexesRest... indexes_rest) {
  return index_first +
         htl::head(extents) *
             get_1d_index_impl(htl::tail(extents), indexes_rest...);
}
}
}

template <
    class... Extents, class... Indexes,
    CONCEPT_REQUIRES(sizeof...(Extents) == sizeof...(Indexes) &&
                     and_c<std::is_convertible<Indexes, index_t>::value...>())>
index_t get_1d_index(const Shape<Extents...>& shape, Indexes... indexes) {
  return detail::shape::get_1d_index_impl(shape.extents(), indexes...);
}
}
}
