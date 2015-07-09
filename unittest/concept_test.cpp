#include <echo/k_array.h>
#include <echo/test.h>

using namespace echo;
using namespace echo::k_array;

TEST_CASE("compatible_extents") {
  CHECK(k_array::concept::compatible_extents<index_t, index_t>());
  CHECK(k_array::concept::compatible_extents<StaticIndex<3>, index_t>());
  CHECK(k_array::concept::compatible_extents<index_t, StaticIndex<3>>());
  CHECK(k_array::concept::compatible_extents<StaticIndex<3>, StaticIndex<3>>());
  CHECK(!k_array::concept::compatible_extents<StaticIndex<8>, StaticIndex<3>>());
}
