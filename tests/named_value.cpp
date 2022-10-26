#include "sgl/named_value.hpp"

#include <catch2/catch.hpp>
inline constexpr auto name = NAME("name");
using namespace sgl::string_view_literals;

TEST_CASE("NamedValue") {
  SECTION("construct from const T&") {
    const int i = 1;
    auto      named_val = sgl::NamedValue(name, i);
    STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(named_val)>,
                                  sgl::NamedValue<sgl::Name<'n', 'a', 'm', 'e'>, int>>);
    STATIC_REQUIRE(
        std::is_same_v<typename decltype(named_val)::name_type, sgl::Name<'n', 'a', 'm', 'e'>>);
    STATIC_REQUIRE(std::is_same_v<typename decltype(named_val)::value_type, int>);
  }
  SECTION("construct from T&&") {
    auto named_val = sgl::NamedValue(name, std::move(1));
    STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(named_val)>,
                                  sgl::NamedValue<sgl::Name<'n', 'a', 'm', 'e'>, int>>);
  }
  SECTION("deduction guide") {
    auto named_val = sgl::NamedValue(name, "hello world");
    STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(named_val)>,
                                  sgl::NamedValue<sgl::Name<'n', 'a', 'm', 'e'>, const char*>>);
  const char lit[]{"Literal"};
  sgl::NamedValue n2 {name,lit};
  STATIC_REQUIRE(std::is_same_v<std::decay_t<decltype(n2)>,
                                  sgl::NamedValue<sgl::Name<'n', 'a', 'm', 'e'>, const char*>>);
  
  }
  SECTION("name()") {
    auto named_val = sgl::NamedValue(name, 1);
    REQUIRE(named_val.name() == "name"_sv);
  }
  SECTION("value()") {
    auto val = GENERATE(1, 2, 3, 4, 55, 10, 255, 123456);
    auto named_val = sgl::NamedValue(name, val);
    REQUIRE(named_val.value() == val);
  }
  SECTION("operator <<=") {
    auto val = GENERATE(1, 2, 3, 4, 55, 10, 255, 123456);
    auto result = name <<= val;
    auto expected = sgl::NamedValue(name, val);
    STATIC_REQUIRE(std::is_same_v<decltype(result), decltype(expected)>);
    REQUIRE(result.value() == expected.value());
  }
}