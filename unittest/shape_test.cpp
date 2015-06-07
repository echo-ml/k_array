#include <echo/k_array/shape.h>
#include <echo/k_array/k_shape.h>
#include <echo/k_array/concept.h>
#include <echo/test.h>
#include <catch.hpp>
#include <cassert>

using namespace echo;
using namespace echo::k_array;

namespace shape_concept = echo::k_array::concept;

struct Shape1 {
  using Dimensionality = StaticIndex<2, 3, 4>;
  using StrideSequence = StaticIndex<1, 2, 6>;
  template <int I>
  IndexInteger dynamic_extent() const {
    return 1;
  }

  template <int I>
  IndexInteger dynamic_stride() const {
    return 1;
  }
};

struct ExtentShape1 {
  using Dimensionality = StaticIndex<2, 3, 4>;
  template <int I>
  IndexInteger dynamic_extent() const {
    return 1;
  }
};

struct FalseSubshape1 {
  using Dimensionality = StaticIndex<2, 3, 4>;
  using StrideSequence = StaticIndex<1, 2>;
  template <int I>
  IndexInteger dynamic_extent() const {
    return 1;
  }

  template <int I>
  IndexInteger dynamic_stride() const {
    return 1;
  }
};

struct Shape2 {
  using Dimensionality =
      StaticIndex<3, 2, Dimension::kDynamic, Dimension::kDynamic>;

  template <int I>
  IndexInteger dynamic_extent() const {
    switch (I) {
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
  using StrideSequence = StaticIndex<2, 5, Stride::kDynamic, Stride::kDynamic>;
  template <int I>
  IndexInteger dynamic_extent() const {
    return 1;
  }

  template <int I>
  IndexInteger dynamic_stride() const {
    switch (I) {
      case 0:
        return 12;
      case 1:
        return 13;
      default:
        assert(0);
    }
  }
};

struct Shape3 {
  using Dimensionality = StaticIndex<2, 3, 4, Dimension::kDynamic>;
  template <int I>
  IndexInteger dynamic_extent() const {
    return 1;
  }
};

ExtentShape1 get_extent_shape(const ExtentShape1& shape) { return shape; }

ExtentShape1 get_extent_shape(const Shape1& shape) { return {}; }

const Shape2& get_extent_shape(const Shape2& shape) { return shape; }

const Shape3& get_extent_shape(const Shape3& shape) { return shape; }

Shape3 get_extent_shape(const Subshape1& shape) { return Shape3(); }

TEST_CASE("shape") {
  Shape1 shape1;
  Shape2 shape2;
  Subshape1 subshape1;

  SECTION("dimensionality concept") {
    REQUIRE(shape_concept::dimensionality<StaticIndex<2, 3, 4>>());

    REQUIRE(
        shape_concept::dimensionality<StaticIndex<3, Dimension::kDynamic>>());
    REQUIRE(
        !shape_concept::dimensionality<StaticIndex<-3, Dimension::kDynamic>>());
    REQUIRE(!shape_concept::dimensionality<StaticIndex<>>());
    REQUIRE(!shape_concept::dimensionality<int>());
  }
  SECTION("stride_sequence concept") {
    REQUIRE(
        shape_concept::stride_sequence<StaticIndex<3, Dimension::kDynamic>>());
    REQUIRE(!shape_concept::stride_sequence<
                StaticIndex<-3, Dimension::kDynamic>>());
    REQUIRE(!shape_concept::stride_sequence<StaticIndex<>>());
    REQUIRE(!shape_concept::stride_sequence<int>());
  }
  SECTION("shape concept") {
    REQUIRE(shape_concept::subshape<Shape1>());
    REQUIRE(!shape_concept::subshape<FalseSubshape1>());
    REQUIRE(!shape_concept::subshape<int>());

    REQUIRE(shape_concept::contiguous_shape<Shape2>());

    REQUIRE(shape_concept::shape<Shape2>());
    REQUIRE(shape_concept::shape<Shape1>());

    REQUIRE(shape_concept::static_shape<Shape1>());
    REQUIRE(!shape_concept::static_shape<Shape2>());
  }
  SECTION("get_extent") {
    type_equal<decltype(get_extent<0>(shape2)), StaticIndex<3>>();
    type_equal<decltype(get_extent<1>(shape2)), StaticIndex<2>>();
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
    type_equal<decltype(get_stride<0>(shape2)), StaticIndex<1>>();
    type_equal<decltype(get_stride<1>(shape2)), StaticIndex<3>>();
    type_equal<decltype(get_stride<2>(shape2)), StaticIndex<6>>();
    REQUIRE(get_stride<3>(shape2) == 42);

    type_equal<decltype(get_stride<0>(subshape1)), StaticIndex<2>>();
    REQUIRE(get_stride<2>(subshape1) == 12);
    REQUIRE(get_stride<3>(subshape1) == 13);
  }

  // SECTION("get_num_elements") {
  //   type_equal<
  //       decltype(get_num_elements(shape1))
  //     , StaticIndex<24>
  //   >();
  //   REQUIRE(get_num_elements(shape2) == 336);
  // }

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

    type_equal<decltype(S1() == S2()), std::true_type>();
    type_equal<decltype(S1() != S2()), std::false_type>();

    type_equal<decltype(S1() == S3()), std::false_type>();
    type_equal<decltype(S1() != S3()), std::true_type>();

    type_equal<decltype(S1() == S4()), std::false_type>();
    type_equal<decltype(S1() != S4()), std::true_type>();

    REQUIRE(S3() == S5(2));
    REQUIRE(!(S3() != S5(2)));

    REQUIRE(!(S3() == S5(5)));
    REQUIRE(S3() != S5(5));

    REQUIRE(S5(7) == S5(7));
    REQUIRE(!(S5(7) != S5(7)));
  }

  SECTION("get_first_shape") {
    // using S1 = KShape<1, 2>;
    // using S2 = KShape<1, 3>;
    // auto shape = get_first_shape(3, 7, S1(), 9, S2());
    // type_equal<
    //     decltype(shape)
    //   , S1
    // >();
  }

  SECTION("assert_any_shapes_match") {
    using S1 = KShape<1, 2>;
    using S2 = KShape<1, 3>;
    using S3 = KShape<1, 2>;

    // assert_any_shapes_match(S1(), 3, 4);
    // assert_any_shapes_match(S1(), S3(), 4);
    // assert_any_shapes_match(S1(), 12, S2(), S3(), 4); //assertion failure
  }
}

TEST_CASE("extent_traits") {
  CHECK(k_array::concept::dynamic_extent<int>());
  CHECK(!k_array::concept::dynamic_extent<StaticIndex<3>>());
  type_equal<k_array::extent_traits::extent_type<Dimension::kDynamic>,
             index_t>();
  type_equal<k_array::extent_traits::extent_type<3>, StaticIndex<3>>();

  static_assert(
      k_array::extent_traits::extent_enum<index_t>() == Dimension::kDynamic,
      "");

  static_assert(k_array::extent_traits::extent_enum<StaticIndex<3>>() == 3, "");
}

TEST_CASE("num_free_dimensions") {
  CHECK(shape_traits::num_free_dimensions<KShape<1>>() == 0);
  CHECK(shape_traits::num_free_dimensions<KShape<1, 3>>() == 1);
  CHECK(shape_traits::num_free_dimensions<KShape<7, 1, 3>>() == 2);
  CHECK(shape_traits::free_dimension<KShape<1, 3>>() == 1);
  CHECK(shape_traits::free_dimension<KShape<3, 1>>() == 0);
}
