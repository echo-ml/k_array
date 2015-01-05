#pragma once

#include <echo/index.h>

namespace echo { namespace k_array {

template<class Tag, int N>
class KSequence {
 public:
  template<
      class... Extents
  >
  KSequence(Extents... extents) 
    : _extents(extents...)
  {
  }

  KSequence(const Index<N>& extents)
    : _extents(extents)
  {}

  template<int I>
  IndexInteger extent() const {
    return get<I>(_extents);
  }

  const Index<N>& extents() const {
    return _extents;
  }

 private:
  Index<N> _extents;
};

template<class Tag>
class KSequence<Tag, 0> {
 public:
  KSequence() {}

  KSequence(const Index<0>&) {}

  Index<0> extents() const {
    return {};
  }
};

}} //end namespace echo::k_array
