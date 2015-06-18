#include <echo/k_array2/dimensionality.h>
#include <echo/k_array2/slice.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("make_subshape") {
  auto d1 = make_dimensionality(1_index, 3, 4_index);
  auto s1 = htl::make_tuple(
    3,
    slice::counted_range(2, 2_index),
    slice::range(1,3)
  );
  auto subdimensions = echo::k_array::detail::slice::get_subdimensions(
    d1, s1
  );
  type_equal<decltype(subdimensions),
             Dimensionality<StaticIndex<2>, index_t>>();
  CHECK(get_extent<1>(subdimensions) == 2);
}
