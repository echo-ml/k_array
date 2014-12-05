#include <echo/k_array/k_array.h>
#include <echo/k_array/k_array_view.h>
#include <echo/k_array/k_shape.h>
#include <echo/k_array/k_array_iteration.h>
#include <catch.hpp>
#include <numeric>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_array") {
  using Shape1 = KShape<2, Dimension::kDynamic, Dimension::kDynamic>;
  Shape1 shape1(3, 2);

  using KArray1 = KArray<double, Shape1>;

  KArray1 k_array1(shape1);
  std::iota(all_begin(k_array1), all_end(k_array1), 0);

  auto k_array_view1 = make_k_array_view(k_array1);

  SECTION("accessor") {
    REQUIRE(k_array1(1, 0, 0) == 1);
    REQUIRE(k_array1(0, 1, 0) == 2);
    REQUIRE(k_array1(0, 0, 1) == 6);

    REQUIRE(k_array_view1(1, 0, 0) == 1);
    REQUIRE(k_array_view1(0, 1, 0) == 2);
    REQUIRE(k_array_view1(0, 0, 1) == 6);
  }
}
