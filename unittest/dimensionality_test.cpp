#include <echo/k_array2/dimensionality.h>
#include <echo/k_array2/shape.h>
#include <echo/k_array2/subshape.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("dimensionality") {
  Dimensionality<StaticIndex<3>, index_t> d1, d2(3_index, 5);
  Dimensionality<StaticIndex<2>, StaticIndex<7>> d3;
  Dimensionality<StaticIndex<3>, StaticIndex<5>> d4;
  DimensionalityC<3, dimension_t::dynamic> d5;

  type_equal<decltype(get_extent<0>(d1)), StaticIndex<3>>();
  CHECK(get_extent<1>(d1) == 0);
  CHECK(get_extent<1>(d2) == 5);

  CHECK(d1 != d2);
  CHECK(!(d3 == d2));
  CHECK(d4 == d2);

  type_equal<decltype(d1), decltype(d5)>();
}

TEST_CASE("get_num_elements") {
  Dimensionality<StaticIndex<3>, StaticIndex<2>> d1;
  Dimensionality<StaticIndex<4>, index_t> d2(4_index, 3);

  type_equal<decltype(get_num_elements(d1)), StaticIndex<6>>();

  CHECK(get_num_elements(d2) == 12);
}

TEST_CASE("num_free_dimensions2") {
  Dimensionality<StaticIndex<3>, index_t> d1;
  Dimensionality<StaticIndex<1>, StaticIndex<2>> d2;
  Dimensionality<StaticIndex<1>, StaticIndex<1>> d3;

  CHECK(dimensionality_traits::num_free_dimensions<decltype(d1)>()==2);
  CHECK(dimensionality_traits::num_free_dimensions<decltype(d2)>()==1);
  CHECK(dimensionality_traits::num_free_dimensions<decltype(d3)>()==0);
}

TEST_CASE("free_dimension2") {
  Dimensionality<StaticIndex<1>, index_t> d1;
  Dimensionality< StaticIndex<2>> d2;

  CHECK(dimensionality_traits::free_dimension<decltype(d1)>()==1);
  CHECK(dimensionality_traits::free_dimension<decltype(d2)>()==0);
}

TEST_CASE("get_stride") {
  auto s1 = make_shape(2_index, 3_index);
  auto s2 = make_shape(2_index, 4, 5);
  ShapeC<2, 3> s3;

  type_equal<decltype(get_stride<0>(s1)), StaticIndex<1>>();
  type_equal<decltype(get_stride<1>(s1)), StaticIndex<2>>();

  type_equal<decltype(get_stride<1>(s2)), StaticIndex<2>>();

  CHECK(get_stride<2>(s2) == 8);

  type_equal<decltype(s1), decltype(s3)>();
}

TEST_CASE("shape - get_1d_index") {
  auto shape1 = make_shape(3_index, 2_index, 7, 8);
  CHECK(get_1d_index(shape1, 1, 0, 5, 3) == 157);
}

TEST_CASE("subshape - get_1d_index") {
  auto subshape1 = make_subshape(make_dimensionality(2, 3, 4, 1),
                                 htl::make_tuple(2_index, 5_index, 12, 13));
  CHECK(get_1d_index(subshape1, 1, 2, 3, 0) == 48);
}