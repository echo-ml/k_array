#include <echo/k_array.h>
#include <echo/test.h>
#include <numeric>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("k_array") {
  using Shape1 = ShapeC<2, dimension_t::dynamic, dimension_t::dynamic>;
  Shape1 shape1(2_index, 3, 2);

  using KArray1 = KArray<double, Shape1>;
  using KArray2 = KArray<double, ShapeC<3, 5, 7>>;

  KArray1 k_array1(shape1), k_array2(shape1);
  std::iota(all_begin(k_array1), all_end(k_array1), 0);

  auto k_array_view1  = make_view(k_array1);
  auto k_array_cview1 = make_cview(k_array1);

  KArray<double, Shape<index_t>> k_array3;
  CHECK(get_num_elements(k_array3) == 0);

  KArray2 k_array4;

  SECTION("accessor") {
    CHECK(k_array1(1, 0, 0) == 1);
    CHECK(k_array1(0, 1, 0) == 2);
    CHECK(k_array1(0, 0, 1) == 6);

    CHECK(k_array_view1(1, 0, 0) == 1);
    CHECK(k_array_view1(0, 1, 0) == 2);
    CHECK(k_array_view1(0, 0, 1) == 6);
  }

  SECTION("assign") {
    k_array2 = k_array1;
    CHECK(k_array2(1, 0, 0) == 1);
  }

  SECTION("move") {
    k_array2 = std::move(k_array1);
    CHECK(k_array2(1, 0, 0) == 1);
  }

  SECTION("copy") {
    copy(k_array1, k_array2);
    // copy(k_array1, k_array4); shouldn't compile
    CHECK(std::equal(all_cbegin(k_array1), all_cend(k_array1),
                     all_cbegin(k_array2)));
  }
}

TEST_CASE("initialization") {
  KArray<double, ShapeC<2, 3>> a1;
  a1 = {{1, 2, 3}, {4, 5, 6}};
  CHECK(a1(0, 0) == 1);
  CHECK(a1(0, 1) == 2);
  CHECK(a1(0, 2) == 3);

  CHECK(a1(1, 0) == 4);
  CHECK(a1(1, 1) == 5);
  CHECK(a1(1, 2) == 6);
}

TEST_CASE("view_initialization") {
  double a[3][2] = {{0}};
  KArrayView<double*, ShapeC<2, 3>> v(&a[0][0], ShapeC<2, 3>());
  v = {{1, 3, 1}, {4, 3, 1}};
  CHECK(a[0][0] == 1);
  CHECK(a[1][0] == 3);
  CHECK(a[2][0] == 1);

  CHECK(a[0][1] == 4);
  CHECK(a[1][1] == 3);
  CHECK(a[2][1] == 1);
}

TEST_CASE("static_k_array") {
  KArray<double, ShapeC<2, 3>, memory::StaticAllocator<double>> a1;
  a1(0, 0) = 2;
  a1(0, 1) = 3;
  a1(0, 2) = 4;

  a1 = {{3, 4, 6}, {2, 1, 7}};
}
