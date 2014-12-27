#include <echo/k_array/k_array.h>
#include <echo/k_array/k_array_view.h>
#include <echo/k_array/k_shape.h>
#include <echo/k_array/k_array_iteration.h>
#include <echo/k_array/list_form_printer.h>
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

  auto k_array_view1 = make_k_array_view(k_array1);

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

  SECTION("move") {
    k_array2 = std::move(k_array1);
    REQUIRE(k_array2(1, 0, 0) == 1);
  }
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
