#include "sgl/array.hpp"

#include <catch2/catch.hpp>


TEST_CASE("array") {
  sgl::Array<int, 4> arr{0, 1, 2, 3};
  REQUIRE(arr.begin() == &arr.data[0]);
  REQUIRE(arr[0] == 0);
  REQUIRE(arr[1] == 1);
  REQUIRE(arr[2] == 2);
  REQUIRE(arr[3] == 3);
  REQUIRE(arr.end()== arr.data + 4);
}