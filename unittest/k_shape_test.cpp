#include <echo/k_array/k_shape.h>
#include <catch.hpp>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_shape") {
  KShape<3, 10, Dimension::kDynamic, 7> shape1(4);
  REQUIRE(is_contiguous_shape<decltype(shape1)>());
  REQUIRE(get_num_elements(shape1) == 840);
}
