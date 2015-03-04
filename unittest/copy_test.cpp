#include <echo/k_array.h>
#include <echo/test.h>
#include <numeric>
#include <algorithm>
using namespace echo::k_array;

TEST_CASE("k_array copy") {
  using DynamicShape = KShape<Dimension::kDynamic, Dimension::kDynamic>;
  DynamicShape shape1(3, 2);

  using KArray1 = KArray<double, DynamicShape>;
  using KArray2 = KArray<double, KShape<2, 3>>;
  using KArray3 = KArray<double, KShape<3, 2>>;

  KArray1 k_array1(shape1);
  KArray2 k_array2; 
  KArray3 k_array3;

  auto k_array_view1 = make_k_array_view(k_array1);
  auto k_array_view2 = make_k_array_view(k_array2);
  auto k_array_view3 = make_k_array_view(k_array3);

  std::iota(all_begin(k_array1), all_end(k_array1), 0);
  std::iota(all_begin(k_array2), all_end(k_array2), 1);
  std::iota(all_begin(k_array3), all_end(k_array3), 1);

  SECTION("copy matrix") {
    copy(k_array1, k_array3);
    REQUIRE(std::equal(all_cbegin(k_array1), all_cend(k_array1), all_cbegin(k_array3)));
    // copy(k_array1, k_array2);  //Assertion Error
  }
  SECTION("copy view") {
    copy(k_array_view1, k_array_view3);
    REQUIRE(
        std::equal(
            all_cbegin(k_array_view1)
          , all_cend(k_array_view1)
          , all_cbegin(k_array_view3)
        )
    );
    
  }
}
