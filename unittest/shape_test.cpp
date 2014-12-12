#include <echo/k_array/shape.h>
#include <echo/k_array/k_shape.h>
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

struct FalseSubshape1 {
  using Dimensionality = StaticIndex<2, 3, 4>;
  using Strides        = StaticIndex<1, 2>;
  template<int I>
  IndexInteger dynamic_extent() const { return 1; }

  template<int I>
  IndexInteger dynamic_stride() const { return 1; }
};

struct Shape2 {
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
  Shape2 shape2;
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
    REQUIRE(!is_subshape<FalseSubshape1>());
    REQUIRE(!is_subshape<int>());

    REQUIRE(is_contiguous_shape<Shape2>());

    REQUIRE(is_shape<Shape2>());
    REQUIRE(is_shape<Shape1>());

    REQUIRE(is_static_shape<Shape1>());
    REQUIRE(!is_static_shape<Shape2>());
  }
  SECTION("get_extent") {
    type_equal<
        decltype(get_extent<0>(shape2))
      , StaticIndex<3>
    >();
    type_equal<
        decltype(get_extent<1>(shape2))
      , StaticIndex<2>
    >();
    REQUIRE(get_extent<2>(shape2) == 7);
    REQUIRE(get_extent<3>(shape2) == 8);
  }
  SECTION("is_static_stride") {
    REQUIRE(is_static_stride<0, Shape2>());
    REQUIRE(is_static_stride<1, Shape2>());
    REQUIRE(is_static_stride<2, Shape2>());
    REQUIRE(!is_static_stride<3, Shape2>());

    REQUIRE(is_static_stride<0, Subshape1>());
    REQUIRE(!is_static_stride<2, Subshape1>());
  }
  SECTION("get_stride") {
    type_equal<
        decltype(get_stride<0>(shape2))
      , StaticIndex<1>
    >();
    type_equal<
        decltype(get_stride<1>(shape2))
      , StaticIndex<3>
    >();
    type_equal<
        decltype(get_stride<2>(shape2))
      , StaticIndex<6>
    >();
    REQUIRE(get_stride<3>(shape2) == 42);

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
    REQUIRE(get_num_elements(shape2) == 336);
  }

  SECTION("get_1d_index") {
    REQUIRE(get_1d_index(shape2, 1, 0, 5, 3) == 157);
    REQUIRE(get_1d_index(subshape1, 1, 2, 3, 0) == 48);
  }

  SECTION("equality") {
    using S1 = KShape<1, 2>;
    using S2 = KShape<1, 2>;
    using S3 = KShape<1, 2, 3>;
    using S4 = KShape<2, 3>;
    using S5 = KShape<1, Dimension::kDynamic, 3>;

    type_equal<
        decltype(S1() == S2())
      , std::true_type
    >();
    type_equal<
        decltype(S1() != S2())
      , std::false_type
    >();


    type_equal<
        decltype(S1() == S3())
      , std::false_type
    >();
    type_equal<
        decltype(S1() != S3())
      , std::true_type
    >();

    type_equal<
        decltype(S1() == S4())
      , std::false_type
    >();
    type_equal<
        decltype(S1() != S4())
      , std::true_type
    >();
    
    REQUIRE(S3() == S5(2));
    REQUIRE(!(S3() != S5(2)));

    REQUIRE(!(S3() == S5(5)));
    REQUIRE(S3() != S5(5));

    REQUIRE(S5(7) == S5(7));
    REQUIRE(!(S5(7) != S5(7)));
  }
}
