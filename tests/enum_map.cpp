#include "sgl/enum_map.hpp"

#include <catch2/catch.hpp>

using namespace sgl::string_view_literals;

enum class Enum {
  e0 = 12345,
  e1 = 30,
  e2 = -12,
  e3 = -42,
};

inline constexpr size_t N = 4;

TEST_CASE("enum_map") {
  constexpr sgl::EnumMap map{sgl::Array{sgl::Pair{Enum::e0, "e0"_sv},
                                        sgl::Pair{Enum::e1, "e1"_sv},
                                        sgl::Pair{Enum::e2, "e2"_sv},
                                        sgl::Pair{Enum::e3, "e3"_sv}}};
  REQUIRE(map.size() == N);

  REQUIRE(map.contains(Enum::e0));
  REQUIRE(map.contains(Enum::e1));
  REQUIRE(map.contains(Enum::e2));
  REQUIRE(map.contains(Enum::e3));
  REQUIRE_FALSE(map.contains((Enum)4));

  REQUIRE(map.contains("e0"_sv));
  REQUIRE(map.contains("e1"_sv));
  REQUIRE(map.contains("e2"_sv));
  REQUIRE(map.contains("e3"_sv));
  REQUIRE_FALSE(map.contains("a0"_sv));
  REQUIRE_FALSE(map.contains("0e"_sv));
  REQUIRE_FALSE(map.contains(sgl::string_view<char>{}));

  REQUIRE(map[Enum::e0] == "e0"_sv);
  REQUIRE(map[Enum::e1] == "e1"_sv);
  REQUIRE(map[Enum::e2] == "e2"_sv);
  REQUIRE(map[Enum::e3] == "e3"_sv);
  REQUIRE(map[(Enum)4] == sgl::string_view<char>{});
  REQUIRE(map["e0"_sv] == Enum::e0);
  REQUIRE(map["e1"_sv] == Enum::e1);
  REQUIRE(map["e2"_sv] == Enum::e2);
  REQUIRE(map["e3"_sv] == Enum::e3);
  SECTION("sgl::enum_map()") {
    constexpr auto map2 =
        sgl::enum_map(Enum::e0, "e0", Enum::e1, "e1", Enum::e2, "e2", Enum::e3, "e3");

    STATIC_REQUIRE(std::is_same_v<decltype(map), decltype(map2)>);
    REQUIRE(map2.size() == N);

    REQUIRE(map2.contains(Enum::e0));
    REQUIRE(map2.contains(Enum::e1));
    REQUIRE(map2.contains(Enum::e2));
    REQUIRE(map2.contains(Enum::e3));
    REQUIRE_FALSE(map2.contains((Enum)4));

    REQUIRE(map2.contains("e0"_sv));
    REQUIRE(map2.contains("e1"_sv));
    REQUIRE(map2.contains("e2"_sv));
    REQUIRE(map2.contains("e3"_sv));
    REQUIRE_FALSE(map2.contains("a0"_sv));
    REQUIRE_FALSE(map2.contains("0e"_sv));
    REQUIRE_FALSE(map2.contains(sgl::string_view<char>{}));

    REQUIRE(map2[Enum::e0] == "e0"_sv);
    REQUIRE(map2[Enum::e1] == "e1"_sv);
    REQUIRE(map2[Enum::e2] == "e2"_sv);
    REQUIRE(map2[Enum::e3] == "e3"_sv);

    REQUIRE(map2["e0"_sv] == Enum::e0);
    REQUIRE(map2["e1"_sv] == Enum::e1);
    REQUIRE(map2["e2"_sv] == Enum::e2);
    REQUIRE(map2["e3"_sv] == Enum::e3);
  }
}