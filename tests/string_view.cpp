#include "sgl/string_view.hpp"

#include <catch2/catch.hpp>

using View = sgl::string_view<char>;
using namespace sgl::string_view_literals;

TEST_CASE("string_view") {
  constexpr char literal[] = "Any string literal";
  constexpr char literal2[] = "Other literal";

  SECTION("constructing from char array") {
    View view(literal);
    REQUIRE(view.size() == (sizeof(literal) - 1));
    REQUIRE(view.data() == &literal[0]);
    REQUIRE(view.begin() == &literal[0]);
    REQUIRE(view.end() == (literal + sizeof(literal) - 1));
  }
  SECTION("constructing from char array") {
    View view((const char*)literal);
    REQUIRE(view.size() == (sizeof(literal) - 1));
    REQUIRE(view.data() == &literal[0]);
    REQUIRE(view.begin() == &literal[0]);
    REQUIRE(view.end() == (literal + sizeof(literal) - 1));
  }
  SECTION("constructing from char array") {
    View view(literal, 5);
    REQUIRE(view.size() == 5);
    REQUIRE(view.data() == &literal[0]);
    REQUIRE(view.begin() == &literal[0]);
    REQUIRE(view.end() == (literal + 5));
  }
  SECTION("assignment operator") {
    View v1(literal);
    View v2(literal2);
    REQUIRE_FALSE(v1.data() == v2.data());
    v1 = v2;
    REQUIRE(v1.data() == v2.data());
    REQUIRE(v1.size() == v2.size());
  }
  SECTION("index operator") {
    View view(literal, 5);
    REQUIRE(view[0] == 'A');
    REQUIRE(view[1] == 'n');
    REQUIRE(view[2] == 'y');
    REQUIRE(view[3] == ' ');
    REQUIRE(view[4] == 's');
  }
  SECTION("equality operator") {
    View v1(literal);
    View v2(literal2);
    REQUIRE_FALSE(v1 == v2);
    v1 = v2;
    REQUIRE(v1 == v2);
    // changing data pointer, although contents are same
    v1 = View("Other literal");
    REQUIRE(v1 == v2);
  }
  SECTION("literal operators") {
    auto v1 = "A literal"_sv;
    REQUIRE(v1.size() == 9);
  }
}