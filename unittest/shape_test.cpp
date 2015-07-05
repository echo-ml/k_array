#include <echo/k_array.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("shape concept") {
  ShapeC<3, 4> s1;
  Subshape<ShapeC<3, 2>, htl::Tuple<StaticIndex<2>, StaticIndex<8>>> s2;

  CHECK(k_array::concept::contiguous_shape<decltype(s1)>());
  CHECK(!k_array::concept::contiguous_shape<decltype(s2)>());
  CHECK(!k_array::concept::subshape<decltype(s1)>());
  CHECK(k_array::concept::subshape<decltype(s2)>());
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
