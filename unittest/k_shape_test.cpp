#include <echo/k_array/k_shape.h>
#include <catch.hpp>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_shape") {
  KShape<3, 10, Dimension::kDynamic, 7> shape1(4);

  SECTION("shape functions") {
    REQUIRE(is_contiguous_shape<decltype(shape1)>());
    REQUIRE(get_num_elements(shape1) == 840);
  }
  SECTION("sub_k_shape") {
    auto subshape1 = shape1.subshape();
    auto subshape2 = subshape1.subshape();
    REQUIRE(get_num_dimensions<decltype(subshape1)>() == 3);
    REQUIRE(get_num_dimensions<decltype(subshape2)>() == 2);

    REQUIRE(get_extent<2>(subshape1) == 4);
  }
}
