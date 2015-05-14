#include <echo/k_array/concept.h>
#include <echo/k_array/k_shape.h>
#include <echo/test.h>
#include <catch.hpp>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_shape") {
  KShape<3, 10, Dimension::kDynamic, 7> shape1(4);

  SECTION("shape functions") {
    REQUIRE(k_array::concept::contiguous_shape<decltype(shape1)>());
    REQUIRE(k_array::concept::contiguous_shape<KShape<3>>());
    REQUIRE(get_num_elements(shape1) == 840);
  }
  SECTION("sub_k_shape") {
    auto subshape1 = shape1.subshape();
    auto subshape2 = subshape1.subshape();
    REQUIRE(get_num_dimensions<decltype(subshape1)>() == 3);
    REQUIRE(get_num_dimensions<decltype(subshape2)>() == 2);

    REQUIRE(get_extent<2>(subshape1) == 4);
  }
  SECTION("make_k_shape") {
    auto shape1 = make_k_shape(3,4);
    REQUIRE(get_extent<0>(shape1) == 3);
    REQUIRE(get_extent<1>(shape1) == 4);
    type_equal<
        decltype(shape1)
      , KShape<Dimension::kDynamic, Dimension::kDynamic>
    >();
    type_equal<
        decltype(shape1)
      , KShapeFromExtents<int, int>
    >();

    auto shape2 = make_k_shape(StaticIndex<3>(), StaticIndex<7>());
    REQUIRE(get_extent<0>(shape2) == 3);
    REQUIRE(get_extent<1>(shape2) == 7);
    type_equal<
        decltype(shape2)
      , KShape<3, 7>
    >();
    type_equal<
        decltype(shape2)
      , KShapeFromExtents<StaticIndex<3>, StaticIndex<7>>
    >();

    auto shape3 = make_k_shape(3, 2_index);
  }
}
