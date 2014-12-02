#include <echo/k_array/shape.h>
#include <catch.hpp>

using namespace echo;
using namespace echo::k_array;

struct Shape1 {
  using Dimensionality = StaticIndex<2, 3, 4>;
  using Strides        = StaticIndex<1, 2, 6>;
  template<int I>
  IndexInteger dynamic_extent() const { return 1; }

  template<int I>
  IndexInteger dynamic_stride() const { return 1; }
};

struct Shape2 {
  using Dimensionality = StaticIndex<2, 3, 4>;
  using Strides        = StaticIndex<1, 2>;
  template<int I>
  IndexInteger dynamic_extent() const { return 1; }

  template<int I>
  IndexInteger dynamic_stride() const { return 1; }
};

TEST_CASE("shape tests") {
  SECTION("dimensionality concept") {
    REQUIRE(is_dimensionality<StaticIndex<3, Dimension::Dynamic>>());
    REQUIRE(!is_dimensionality<StaticIndex<-3, Dimension::Dynamic>>());
    REQUIRE(!is_dimensionality<StaticIndex<>>());
    REQUIRE(!is_dimensionality<int>());
  }
  SECTION("stride_sequence concept") {
    REQUIRE(is_stride_sequence<StaticIndex<3, Dimension::Dynamic>>());
    REQUIRE(!is_stride_sequence<StaticIndex<-3, Dimension::Dynamic>>());
    REQUIRE(!is_stride_sequence<StaticIndex<>>());
    REQUIRE(!is_stride_sequence<int>());
  }
  SECTION("shape concept") {
    REQUIRE(is_shape<Shape1>());
    REQUIRE(!is_shape<Shape2>());
    REQUIRE(!is_shape<int>());
  }
}
