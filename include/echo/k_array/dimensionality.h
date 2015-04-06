#pragma once

#include <echo/index.h>

namespace echo { namespace k_array {

///////////////
// Dimension //
///////////////

struct Dimension {
  static const IndexInteger kDynamic = -1;
  using Dynamic = StaticIndex<kDynamic>;
};

////////////
// Stride //
////////////

struct Stride {
  static const IndexInteger kDynamic = -1;
  using Dynamic = StaticIndex<kDynamic>;
};

///////////
// Slice //
///////////

struct Slice {
  static const IndexInteger kFull = -2;
  using Full = StaticIndex<kFull>;

  static const int kNull = -3;
  using Null = StaticIndex<kNull>;
};

}  // namespace k_array
}  // namespace echo
