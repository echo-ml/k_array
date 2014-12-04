#include <echo/k_array/k_array.h>
#include <echo/k_array/k_shape.h>
#include <catch.hpp>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_array") {
  using Shape1 = KShape<2, Dimension::kDynamic, Dimension::kDynamic>;
  Shape1 shape1(3, 7);

  using KArray1 = KArray<double, Shape1>;

  KArray1 k_array1(shape1);
  k_array1(1, 2, 3) = 8;
}
