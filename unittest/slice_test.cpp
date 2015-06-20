#include <echo/k_array2/dimensionality.h>
#include <echo/k_array2/shape.h>
#include <echo/k_array2/slice.h>
#include <echo/k_array2/make_subshape.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("make_subshape") {
  auto d1 = make_dimensionality(1_index, 3, 4_index);
  auto s1 =
      htl::make_tuple(3, slice::counted_range(2, 2_index), slice::range(1, 3));
  auto subdimensions =
      echo::k_array::detail::make_subshape::get_subdimensions(d1, s1);
  type_equal<decltype(subdimensions),
             Dimensionality<StaticIndex<2>, index_t>>();
  CHECK(get_extent<1>(subdimensions) == 2);
}

TEST_CASE("subdimension_indexes") {
  auto s1 =
      htl::make_tuple(3, slice::counted_range(2, 2_index), slice::range(1, 3));
  using T1 =
      echo::k_array::detail::make_subshape::SubdimensionIndexes<decltype(s1)>;
  type_equal<T1, htl::index_sequence<1, 2>>();
}

TEST_CASE("get_strides") {
  auto shape1 = make_shape(2_index, 3_index, 5, 7);

  auto strides1 = echo::k_array::detail::make_subshape::get_strides(
      htl::index_sequence<1, 3>(), shape1);
  type_equal<decltype(strides1), htl::Tuple<StaticIndex<2>, index_t>>();
  CHECK(htl::get<1>(strides1) == 30);

  auto strides2 = echo::k_array::detail::make_subshape::get_strides(
      htl::index_sequence<0>(), shape1);
  type_equal<decltype(strides2), htl::Tuple<StaticIndex<1>>>();
}

TEST_CASE("make_subshape2") {
  auto shape1 = make_shape(2_index, 3_index, 5, 7);

  auto subshape1 =
      make_subshape(shape1, 1, slice::counted_range(0, 2_index),
                    slice::range(2, 3), slice::counted_range(3, 2_index));

  type_equal<decltype(subshape1),
             Subshape<Dimensionality<StaticIndex<2>, index_t, StaticIndex<2>>,
                      htl::Tuple<StaticIndex<2>, StaticIndex<6>, index_t>>>();
  CHECK(get_extent<1>(subshape1) == 1);
  CHECK(get_stride<2>(subshape1) == 30);

  auto subshape2 =
    make_subshape(shape1, slice::all, slice::all, 2, 3);

  type_equal<decltype(subshape2), Shape<StaticIndex<2>, StaticIndex<3>>>();
}

TEST_CASE("make_subshape3") {
  auto shape1 = make_subshape(
    make_dimensionality(3_index, 2_index),
    htl::make_tuple(5_index, 20)
  );
  auto subshape1 =
      make_subshape(shape1, slice::counted_range(0, 2_index), slice::all);

  type_equal<decltype(subshape1),
             Subshape<Dimensionality<StaticIndex<2>, StaticIndex<2>>,
                      htl::Tuple<StaticIndex<5>, index_t>>>();
  CHECK(get_stride<1>(subshape1) == 20);
}
