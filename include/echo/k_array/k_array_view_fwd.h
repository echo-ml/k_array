#pragma once

#include <echo/memory.h>

namespace echo {
namespace k_array {

template <class Pointer, class Shape,
          class MemoryBackendTag = echo::memory::memory_backend_tag>
class KArrayView;
}
}
