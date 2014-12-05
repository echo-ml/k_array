cmake_minimum_required(VERSION 3.0)

project("echo-k_array")

enable_testing()

include(Echo/Test)

include_directories(${CMAKE_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

file(GLOB headers
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/*.h"
)

file(GLOB headers-k_array
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/echo/k_array/*.h"
)

file(GLOB unittests
  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/unittest/*_test.cpp"
)


install(FILES ${headers} DESTINATION include/echo)
install(FILES ${headers-k_array} DESTINATION include/echo/k_array)

add_catch_test(k_array-test ${unittests})

# add_subdirectory(unittest)
