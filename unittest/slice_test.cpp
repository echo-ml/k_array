#include <echo/k_array/dimensionality.h>
#include <echo/k_array/shape.h>
#include <echo/k_array/slice.h>
#include <echo/k_array/make_subshape.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

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

  auto subshape2 = make_subshape(shape1, slice::all, slice::all, 2, 3);

  type_equal<decltype(subshape2), Shape<StaticIndex<2>, StaticIndex<3>>>();
}

TEST_CASE("make_subshape3") {
  auto shape1 = make_subshape(make_dimensionality(3_index, 2_index),
                              make_strides(5_index, 20));
  auto subshape1 =
      make_subshape(shape1, slice::counted_range(0, 2_index), slice::all);

  type_equal<decltype(subshape1),
             Subshape<Dimensionality<StaticIndex<2>, StaticIndex<2>>,
                      htl::Tuple<StaticIndex<5>, index_t>>>();
  CHECK(get_stride<1>(subshape1) == 20);
}
