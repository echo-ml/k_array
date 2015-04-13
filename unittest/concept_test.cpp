#include <echo/k_array/concept.h>
#include <echo/k_array/k_shape.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("concept") {
  REQUIRE(!k_array::concept::static_extent<int>());
  REQUIRE(k_array::concept::static_extent<StaticIndex<3>>());
  REQUIRE(!k_array::concept::dynamic_extent<StaticIndex<3>>());
  REQUIRE(k_array::concept::dynamic_extent<int>());
  REQUIRE(k_array::concept::extent<int>());

  REQUIRE(k_array::concept::k_shape<3, KShape<2, 2, 7>>());
  REQUIRE(!k_array::concept::k_shape<2, KShape<2, 2, 7>>());
  REQUIRE(!k_array::concept::k_shape<2, int>());
}
