#include <echo/k_array/k_array.h>
#include <echo/k_array/k_array_view.h>
#include <echo/k_array/k_subarray_view.h>
#include <echo/k_array/k_shape.h>
#include <echo/k_array/k_subshape.h>
#include <echo/k_array/k_array_iteration.h>
#include <echo/k_array/list_form_printer.h>
#include <echo/k_array/slice.h>
#include <echo/test.h>
#include <catch.hpp>
#include <numeric>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_array") {
  using Shape1 = KShape<2, Dimension::kDynamic, Dimension::kDynamic>;
  Shape1 shape1(3, 2);

  using KArray1 = KArray<double, Shape1>;

  KArray1 k_array1(shape1), k_array2(shape1);
  std::iota(all_begin(k_array1), all_end(k_array1), 0);

  auto k_array_view1  = make_k_array_view(k_array1);
  auto k_array_cview1 = make_k_array_cview(k_array1);

  KArray<double, KShape<Dimension::kDynamic>> k_array3;
  REQUIRE(get_num_elements(k_array3) == 0);

  SECTION("accessor") {
    REQUIRE(k_array1(1, 0, 0) == 1);
    REQUIRE(k_array1(0, 1, 0) == 2);
    REQUIRE(k_array1(0, 0, 1) == 6);

    REQUIRE(k_array_view1(1, 0, 0) == 1);
    REQUIRE(k_array_view1(0, 1, 0) == 2);
    REQUIRE(k_array_view1(0, 0, 1) == 6);
  }

  SECTION("assign") {
    k_array2 = k_array1;
    REQUIRE(k_array2(1, 0, 0) == 1);
  }

  SECTION("copy_construct") {
    auto k_array_copy = k_array1;
  }
  SECTION("copy_construct other_allocator") {
    KArray<double, Shape1, SimdAllocator<double>> k_array_copy(k_array1);
  }
  SECTION("move") {
    k_array2 = std::move(k_array1);
    REQUIRE(k_array2(1, 0, 0) == 1);
  }
}

TEST_CASE("slice") {
  using Shape1 = KShape<2, Dimension::kDynamic, Dimension::kDynamic>;
  Shape1 shape1(3,4);

  auto subshape1 = slice(shape1, 1, Slice::Null(), StaticIndex<2>());

  type_equal<
      decltype(subshape1)
    , KSubshape<KShape<Dimension::kDynamic, 2>, KShapeStrides<1, Stride::kDynamic>>
    // , KSubshape<KShape<Dimension::kDynamic, 2>, 1, Stride::kDynamic>
  >();

  REQUIRE(get_extent<0>(subshape1) == 1);
  REQUIRE(get_extent<1>(subshape1) == 2);
  REQUIRE(get_stride<0>(subshape1) == 1);
  REQUIRE(get_stride<1>(subshape1) == 6);

  auto subshape2 = slice(subshape1, Slice::Null(), Slice::Full());
}

TEST_CASE("printers") {
  std::ostringstream oss;
  using Shape1 = KShape<Dimension::kDynamic>;
  using Shape2 = KShape<Dimension::kDynamic, Dimension::kDynamic>;
  using Shape3 = KShape<Dimension::kDynamic, Dimension::kDynamic, Dimension::kDynamic>;

  KArray<double, Shape1> k_array1(Shape1(3));
  KArray<double, Shape2> k_array2(Shape2(3, 2));
  KArray<double, Shape3> k_array3(Shape3(4, 3, 2));

  std::iota(all_begin(k_array1), all_end(k_array1), 0);
  std::iota(all_begin(k_array2), all_end(k_array2), 0);
  std::iota(all_begin(k_array3), all_end(k_array3), 0);

  oss << ListForm() << k_array1 << "\n";
  oss << ListForm() << k_array2 << "\n";
  oss << ListForm() << k_array3 << "\n";
}

TEST_CASE("static k_array") {
  KArray<double, KShape<2 ,3>, StaticAllocator<double>> a1;
  a1 = {1, 2, 3, 4, 5, 6};
  REQUIRE(a1(1, 0) == 2);

  KArray<double, KShape<2 ,3>, StaticAllocator<double>> a2;
  std::iota(all_begin(a2), all_end(a2), 2);
  a1 = a2;
  REQUIRE(a1(1, 0) == 3);
}
