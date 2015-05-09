#pragma once

#include <echo/index.h>
#include <algorithm>

namespace echo {
namespace k_array {

///////////////
// KSequence //
///////////////

template <class Tag, int N>
class KSequence {
 public:
  template <class... Extents, CONCEPT_REQUIRES(sizeof...(Extents) == N)>
  explicit KSequence(Extents... extents)
      : _extents(extents...) {}

  explicit KSequence(const Index<N>& extents) : _extents(extents) {}

  KSequence() { std::fill_n(_extents.begin(), N, 0); }

  template <int I>
  IndexInteger extent() const {
    return get<I>(_extents);
  }

  const Index<N>& extents() const { return _extents; }

 private:
  Index<N> _extents;
};

template <class Tag>
class KSequence<Tag, 0> {
 public:
  KSequence() {}

  KSequence(const Index<0>&) {}

  Index<0> extents() const { return {}; }
};
}  // namespace k_array
}  // namespace echo
