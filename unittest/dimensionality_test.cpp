#include <echo/k_array.h>
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

  type_equal<decltype(d3 == d2), htl::integral_constant<bool, false>>();

  type_equal<decltype(d1), decltype(d5)>();
}

TEST_CASE("dimensionality concept") {
  DimensionalityC<dimension_t::dynamic, 3> d1;
  DimensionalityC<2, 1, 3> d2;
  ShapeC<3, 4> s1;
  Subshape<ShapeC<3, 2>, htl::Tuple<StaticIndex<2>, StaticIndex<8>>> s2;

  CHECK(k_array::concept::dimensionality<decltype(d1)>());
  CHECK(k_array::concept::dimensionality_<2, decltype(d1)>());
  CHECK(!k_array::concept::dimensionality<decltype(s1)>());

  CHECK(k_array::concept::static_dimensionality<decltype(d2)>());
  CHECK(!k_array::concept::static_dimensionality<decltype(d1)>());
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

  CHECK(dimensionality_traits::num_free_dimensions<decltype(d1)>() == 2);
  CHECK(dimensionality_traits::num_free_dimensions<decltype(d2)>() == 1);
  CHECK(dimensionality_traits::num_free_dimensions<decltype(d3)>() == 0);

  ShapeC<3, dimension_t::dynamic> s1;
  CHECK(shape_traits::num_free_dimensions<decltype(s1)>() == 2);
}

TEST_CASE("free_dimension2") {
  Dimensionality<StaticIndex<1>, index_t> d1;
  Dimensionality<StaticIndex<2>> d2;

  CHECK(dimensionality_traits::free_dimension<decltype(d1)>() == 1);
  CHECK(dimensionality_traits::free_dimension<decltype(d2)>() == 0);
}

