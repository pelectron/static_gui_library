#include "sgl/name.hpp"

#include <catch2/catch.hpp>
#include <cstring>

inline constexpr auto name = NAME("name1");

TEST_CASE("name") {
  REQUIRE(name.to_view() == sgl::string_view<char>("name1"));
  STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(name)>, sgl::Name<'n', 'a', 'm', 'e', '1'>>);
}