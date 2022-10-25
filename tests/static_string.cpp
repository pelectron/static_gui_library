#include "sgl/static_string.hpp"

#include <catch2/catch.hpp>
#include <cstring>

using namespace sgl::string_view_literals;
constexpr size_t N = 20;
using String = sgl::static_string<char, N>;
constexpr char literal1[] = "literal 1";
constexpr char literal2[] = "the second literal! ! ! ! !";

TEST_CASE("static_string") {
  static const char array[N + 1]{0};
  SECTION("default constructed string") {
    String str{};
    REQUIRE(str.size() == 0);
    REQUIRE(*str.c_str() == 0);
    REQUIRE(strnlen(str.c_str(), N) == 0);
    REQUIRE(str.begin() == str.end());
    REQUIRE(str.is_empty());
  }
  SECTION("construct with size and value") {
    String str{5, 'X'};
    REQUIRE(str.size() == 5);
    REQUIRE(strnlen(str.c_str(), N) == 5);
    REQUIRE(strncmp(str.c_str(), "XXXXX", N) == 0);
    REQUIRE_FALSE(str.is_empty());
  }
  SECTION("construct from literal") {
    // literal smaller than capacity
    String str1{literal1};
    REQUIRE(str1.size() == (sizeof(literal1) - 1));
    REQUIRE(strnlen(str1.c_str(), N) == (sizeof(literal1) - 1));
    REQUIRE(strncmp(str1.c_str(), literal1, N) == 0);
    REQUIRE_FALSE(str1.is_empty());

    // literals bigger than capacity
    String str2{literal2};
    REQUIRE(str2.size() == N);
    REQUIRE(strnlen(str2.c_str(), N) == N);
    REQUIRE(strncmp(str2.c_str(), literal2, N) == 0);
    REQUIRE_FALSE(str2.is_empty());
  }
  SECTION("construct from pointer and size") {
    // literal smaller than capacity
    String str1{(const char*)literal1, sizeof(literal1) - 1};
    REQUIRE(str1.size() == (sizeof(literal1) - 1));
    REQUIRE(strnlen(str1.c_str(), N) == (sizeof(literal1) - 1));
    REQUIRE(strncmp(str1.c_str(), literal1, N) == 0);
    REQUIRE_FALSE(str1.is_empty());

    // literals bigger than capacity
    String str2{(const char*)literal2, sizeof(literal2) - 1};
    REQUIRE(str2.size() == N);
    REQUIRE(strnlen(str2.c_str(), N) == N);
    REQUIRE(strncmp(str2.c_str(), literal2, N) == 0);
    REQUIRE_FALSE(str2.is_empty());
  }
  SECTION("construct from literal") {
    // literal smaller than capacity
    String str1{sgl::string_view<char>(literal1)};
    REQUIRE(str1.size() == (sizeof(literal1) - 1));
    REQUIRE(strnlen(str1.c_str(), N) == (sizeof(literal1) - 1));
    REQUIRE(strncmp(str1.c_str(), literal1, N) == 0);
    REQUIRE_FALSE(str1.is_empty());

    // literals bigger than capacity
    String str2{sgl::string_view<char>(literal2)};
    REQUIRE(str2.size() == N);
    REQUIRE(strnlen(str2.c_str(), N) == N);
    REQUIRE(strncmp(str2.c_str(), literal2, N) == 0);
    REQUIRE_FALSE(str2.is_empty());
  }
  SECTION("operator []") {
    String str{literal1};
    REQUIRE((&str[0]) == str.begin());
    REQUIRE((&str[3]) == str.begin() + 3);
  }
  GIVEN("a static_string") {
    auto   view = GENERATE(sgl::string_view<char>{},
                         "string1"_sv,
                         "string2134"_sv,
                         "string a"_sv,
                         "A very long string that does not fit into the tested static string"_sv);
    String str{view};
    THEN("c_str, data and begin return the same address") {
      REQUIRE(str.c_str() == str.data());
      REQUIRE(str.c_str() == str.begin());
    }
    AND_THEN("begin + size is equal to end") { REQUIRE(str.begin() + str.size() == str.end()); }

    WHEN("appending a character") {
      const auto old_size = str.size();
      str.append('x');
      if (old_size < str.capacity()) {
        THEN("the size increases by one") { REQUIRE(str.size() == old_size + 1); }
        AND_THEN("the character gets appended") {
          REQUIRE(str[old_size] == 'x');
          REQUIRE(str[str.size()] == 0);
        }
      } else {
        THEN("the content remains unchanged") {
          REQUIRE(str.size() == old_size);
          REQUIRE(str[str.size()] == 0);
        }
      }
    }
    WHEN("appending a string") {
      const auto old_size = str.size();
      REQUIRE(str.size() == (view.size() > N ? N : view.size()));
      REQUIRE(str[old_size] == 0);
      const auto appended_size = str.append("xxx", 3);
      REQUIRE((old_size + appended_size) == str.size());
      REQUIRE(strncmp(str.data() + old_size, "xxx", appended_size) == 0);
      REQUIRE(str[str.size()] == 0);
    }
    WHEN("resetting the string") {
      str.reset();
      THEN("the content is cleared") { REQUIRE(memcmp(str.data(), array, N + 1) == 0); }
      AND_THEN("the size is 0") {
        REQUIRE(strnlen(str.data(), N) == 0);
        REQUIRE(str.size() == 0);
      }
    }
    WHEN("resizing bigger") {
      const auto old_size = str.size();
      str.resize(old_size + 1);
      if (old_size != str.capacity()) {
        REQUIRE(str.size() == old_size + 1);
        REQUIRE(str[old_size] == '\0');
      }
    }
    WHEN("resizing smaller") {
      const auto old_size = str.size();
      const auto new_size = old_size == 0 ? 0 : old_size - 1;
      str.resize(new_size);
      REQUIRE(str.size() == new_size);
      REQUIRE(str[str.size()] == '\0');
    }
  }
  SECTION("equality operator") {
    auto   v1 = GENERATE(sgl::string_view<char>{},
                       "string1"_sv,
                       "string2134"_sv,
                       "string a"_sv,
                       "A very long string that does not fit into the tested static string"_sv);
    
    auto   v2 = GENERATE(sgl::string_view<char>{},
                       "string1"_sv,
                       "string2134"_sv,
                       "string a"_sv,
                       "A very long string that does not fit into the tested static string"_sv);
    String s1(v1);
    String s2(v2);
    if (strncmp(s1.data(), s2.data(), N) != 0) {
      REQUIRE_FALSE(s1 == s2);
    } else {
      REQUIRE(s1 == s2);
    }
  }
}