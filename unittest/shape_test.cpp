#include <echo/k_array/shape.h>
#include <echo/test.h>
#include <catch.hpp>
#include <cassert>

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

struct Shape3 {
  using Dimensionality = StaticIndex<3, 2, Dimension::kDynamic, Dimension::kDynamic>;

  template<int I>
  IndexInteger dynamic_extent() const { 
    switch(I) {
      case 0:
        return 7;
      case 1:
        return 8;
      default:
        assert(0);
    }
  }
};

struct Subshape1 {
  using Dimensionality = StaticIndex<2, 3, 4, Dimension::kDynamic>;
  using Strides        = StaticIndex<2, 5, Stride::kDynamic, Stride::kDynamic>;
  template<int I>
  IndexInteger dynamic_extent() const { return 1; }

  template<int I>
  IndexInteger dynamic_stride() const { 
    switch(I) {
      case 0:
        return 12;
      case 1:
        return 13;
      default:
        assert(0);
    }
  }
};

TEST_CASE("shape") {
  Shape1 shape1;
  Shape3 shape3;
  Subshape1 subshape1;

  SECTION("dimensionality concept") {
    REQUIRE(is_dimensionality<StaticIndex<3, Dimension::kDynamic>>());
    REQUIRE(!is_dimensionality<StaticIndex<-3, Dimension::kDynamic>>());
    REQUIRE(!is_dimensionality<StaticIndex<>>());
    REQUIRE(!is_dimensionality<int>());
  }
  SECTION("stride_sequence concept") {
    REQUIRE(is_stride_sequence<StaticIndex<3, Dimension::kDynamic>>());
    REQUIRE(!is_stride_sequence<StaticIndex<-3, Dimension::kDynamic>>());
    REQUIRE(!is_stride_sequence<StaticIndex<>>());
    REQUIRE(!is_stride_sequence<int>());
  }
  SECTION("shape concept") {
    REQUIRE(is_subshape<Shape1>());
    REQUIRE(!is_subshape<Shape2>());
    REQUIRE(!is_subshape<int>());

    REQUIRE(is_contiguous_shape<Shape3>());

    REQUIRE(is_shape<Shape3>());
    REQUIRE(is_shape<Shape1>());

    REQUIRE(is_static_shape<Shape1>());
    REQUIRE(!is_static_shape<Shape3>());
  }
  SECTION("get_extent") {
    type_equal<
        decltype(get_extent<0>(shape3))
      , StaticIndex<3>
    >();
    type_equal<
        decltype(get_extent<1>(shape3))
      , StaticIndex<2>
    >();
    REQUIRE(get_extent<2>(shape3) == 7);
    REQUIRE(get_extent<3>(shape3) == 8);
  }
  SECTION("is_static_stride") {
    REQUIRE(is_static_stride<0, Shape3>());
    REQUIRE(is_static_stride<1, Shape3>());
    REQUIRE(is_static_stride<2, Shape3>());
    REQUIRE(!is_static_stride<3, Shape3>());

    REQUIRE(is_static_stride<0, Subshape1>());
    REQUIRE(!is_static_stride<2, Subshape1>());
  }
  SECTION("get_stride") {
    type_equal<
        decltype(get_stride<0>(shape3))
      , StaticIndex<1>
    >();
    type_equal<
        decltype(get_stride<1>(shape3))
      , StaticIndex<3>
    >();
    type_equal<
        decltype(get_stride<2>(shape3))
      , StaticIndex<6>
    >();
    REQUIRE(get_stride<3>(shape3) == 42);

    type_equal<
        decltype(get_stride<0>(subshape1))
      , StaticIndex<2>
    >();
    REQUIRE(get_stride<2>(subshape1) == 12);
    REQUIRE(get_stride<3>(subshape1) == 13);
  }

  SECTION("get_num_elements") {
    type_equal<
        decltype(get_num_elements(shape1))
      , StaticIndex<24>
    >();
    REQUIRE(get_num_elements(shape3) == 336);
  }
}
