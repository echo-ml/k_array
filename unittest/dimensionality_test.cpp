#include <echo/k_array/iteration.h>
#include <echo/k_array/dimensionality.h>
#include <echo/k_array/shape.h>
#include <echo/k_array/subshape.h>
#include <echo/k_array/k_array.h>
#include <echo/k_array/k_array_view.h>
#include <echo/k_array/shaped.h>
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

TEST_CASE("concept2") {
  DimensionalityC<dimension_t::dynamic, 3> d1;
  DimensionalityC<2, 1, 3> d2;
  ShapeC<3, 4> s1;
  Subshape<ShapeC<3, 2>, htl::Tuple<StaticIndex<2>, StaticIndex<8>>> s2;

  CHECK(k_array::concept::dimensionality<decltype(d1)>());
  CHECK(!k_array::concept::dimensionality<decltype(s1)>());

  CHECK(k_array::concept::static_dimensionality<decltype(d2)>());
  CHECK(!k_array::concept::static_dimensionality<decltype(d1)>());

  CHECK(k_array::concept::contiguous_shape<decltype(s1)>());
  CHECK(!k_array::concept::contiguous_shape<decltype(s2)>());
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

struct Shaped1 {
  auto shape() const { return ShapeC<3, 5>(); }
};

struct Dimensioned1 {
  auto dimensionality() const { return DimensionalityC<7, 8>(); }
};

TEST_CASE("shaped") {
  Shaped1 s1;
  Dimensioned1 d1;

  CHECK(k_array::concept::dimensioned<Shaped1>());
  CHECK(k_array::concept::dimensioned<Dimensioned1>());
  CHECK(!k_array::concept::shaped<Dimensioned1>());
  CHECK(k_array::concept::shaped<Shaped1>());
  CHECK(!k_array::concept::dimensioned<int>());
  CHECK(!k_array::concept::shaped<int>());

  CHECK(get_num_elements(s1) == 15);
  CHECK(get_num_elements(d1) == 56);
  CHECK(get_extent<1>(d1) == 8);
  CHECK(get_stride<1>(s1) == 3);
}
