#include "catch2/catch.hpp"
#include "sgl/fix_point.hpp"
#include <iostream>
TEST_CASE("unsigned_fixed construction") {
  SECTION("constructing from double") {
    sgl::unsigned_fixed<4, 4> v1(1.5);
    REQUIRE(v1.value() == 0b00011000);
    sgl::unsigned_fixed<4, 2> v2(15.75);
    REQUIRE(v2.value() == 0b00111111);
    sgl::unsigned_fixed<4, 2> v3(0.0);
    REQUIRE(v3.value() == 0);
  }
  SECTION("constructing from float") {
    sgl::unsigned_fixed<4, 4> v1(1.5f);
    REQUIRE(v1.value() == 0b00011000);
    sgl::unsigned_fixed<4, 2> v2(15.75f);
    REQUIRE(v2.value() == 0b00111111);
    sgl::unsigned_fixed<4, 2> v3(0.0f);
    REQUIRE(v3.value() == 0);
  }
  SECTION("construct from raw value") {
    sgl::unsigned_fixed<4, 4> v1(uint8_t{0x11});
    REQUIRE(v1.value() == 0x11);
    sgl::unsigned_fixed<20, 10> v2(0x1ABCu);
    REQUIRE(v2.value() == 0x1ABCu);
  }
  SECTION("unused bits get cleared") {
    sgl::unsigned_fixed<20, 10> v3(0xFFFFFFFFu);
    REQUIRE(v3.value() == 0x3FFFFFFFu);
  }
  SECTION("construct from negative floating point") {}
}

TEST_CASE("unsigned_fixed::integer()") {
  sgl::unsigned_fixed<20, 10> v1(20.75);
  REQUIRE(v1.integer() == 20);

  sgl::unsigned_fixed<20, 10> v2(123456.75);
  REQUIRE(v2.integer() == 123456);
}

TEST_CASE("unsigned_fixed::fraction()") {
  sgl::unsigned_fixed<20, 10> v1(20.75);
  REQUIRE(v1.fraction() == 0b1100000000);

  sgl::unsigned_fixed<20, 10> v2(12345678.75);
  REQUIRE(v2.fraction() == 0b1100000000);
}

TEST_CASE("unsigned_fixed to to_float") {
  auto floats = GENERATE(0.0f, 1.0f, 1.25f, 1.5f, 12.5f, 13.0f, 15.75f);
  REQUIRE(to_float(sgl::unsigned_fixed<4, 2>{floats}) == floats);
  REQUIRE(to_float(sgl::unsigned_fixed<10, 22>{floats}) == floats);
}

TEST_CASE("unsigned_fixed to_double") {
  auto doubles = GENERATE(0.0, 1.0, 1.25, 1.5, 12.5, 13.0, 15.75);
  REQUIRE(to_float(sgl::unsigned_fixed<4, 2>{doubles}) == doubles);
  REQUIRE(to_float(sgl::unsigned_fixed<10, 22>{doubles}) == doubles);
}

TEST_CASE("unsigned_fixed addition") {
  sgl::unsigned_fixed<4, 4> v1(1.5);
  auto                      v2 = v1 + v1;
  REQUIRE(to_double(v2) == 3.0);
}

TEST_CASE("unsigned_fixed subtraction") {
  sgl::unsigned_fixed<4, 2> v1(0.0);
  auto                      v2 = v1 + sgl::unsigned_fixed<4, 2>(0.25);
  REQUIRE(v2.value() == 0b01);
}

TEST_CASE("unsigned_fixed multiplication") {
  sgl::unsigned_fixed<4, 2>   v1(3.75);
  sgl::unsigned_fixed<10, 10> v2(20.25);
  REQUIRE(to_double(v1 * v2) == (3.75 * 20.25));
}
TEST_CASE("unsigned_fixed division") {
  // TODO: test division by 0
  auto                        float1 = GENERATE(1.0, 2.25, 3.5, 21.5, 16.75);
  sgl::unsigned_fixed<10, 10> v1(float1);
  sgl::unsigned_fixed<10, 10> v2(float1);
  std::cout << to_double(v1 / v2) << std::endl;
  // REQUIRE(to_double(v1 / v2) == to_double(decltype(v1 / v2)(float1 / float2)));
}

TEST_CASE("signed_fixed construction") {
  SECTION("constructing from double") {
    sgl::unsigned_fixed<4, 4> v1(1.5);
    REQUIRE(v1.value() == 0b00011000);

    sgl::signed_fixed<4, 4> v4(-1.5);
    REQUIRE(v4.value() == 0b11101000);

    sgl::signed_fixed<5, 2> v2(15.75);
    REQUIRE(v2.value() == 0b00111111);

    sgl::signed_fixed<4, 2> v3(0.0);
    REQUIRE(v3.value() == 0);
  }
  SECTION("constructing from float") {
    sgl::signed_fixed<4, 4> v1(1.5f);
    REQUIRE(v1.value() == 0b00011000);

    sgl::signed_fixed<4, 4> v4(-1.5f);
    REQUIRE(v4.value() == 0b11101000);

    sgl::signed_fixed<4, 2> v2(15.75f);
    REQUIRE(v2.value() == 0b00111111);

    sgl::signed_fixed<4, 2> v3(0.0f);
    REQUIRE(v3.value() == 0);
  }

  SECTION("construct from raw value") {
    sgl::unsigned_fixed<4, 4> v1(uint8_t{0x11});
    REQUIRE(v1.value() == 0x11);

    sgl::signed_fixed<20, 10> v2(0x1ABCu);
    REQUIRE(v2.value() == 0x1ABCu);
  }
  SECTION("unused bits get cleared") {
    sgl::signed_fixed<20, 10> v3(0xFFFFFFFFu);
    REQUIRE(v3.value() == 0x3FFFFFFFu);
  }
}
TEST_CASE("signed_fixed addition") {}
TEST_CASE("signed_fixed subtraction") {}
TEST_CASE("signed_fixed multiplication") {}
TEST_CASE("signed_fixed division") {}

TEST_CASE("to_signed") {}
TEST_CASE("to_unsigned") {}