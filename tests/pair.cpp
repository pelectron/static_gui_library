#include "sgl/pair.hpp"

#include <catch2/catch.hpp>
#include <string>
using namespace std::string_literals;

TEST_CASE("Pair constructor") {

  SECTION("construct from both params") {
    sgl::Pair<int, int> p{2, 3};
    REQUIRE(p.first == 2);
    REQUIRE(p.second == 3);
  }
  SECTION("construct from moved params") {
    sgl::Pair<std::string, std::string> p{std::move("hello"s), std::move("world"s)};
    REQUIRE(p.first == "hello");
    REQUIRE(p.second == "world");
  }
  SECTION("deduction guide") {
    auto p = sgl::Pair{"hello", "world"};
    STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(p)>, sgl::Pair<char*, char*>>);
  }
}