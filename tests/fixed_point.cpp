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
  auto                        float1 = GENERATE(1.0, 2.25, 3.5, 21.5, 16.75, 7.4444444444);
  auto                        float2 = GENERATE(2.25, 3.5, 21.5, 16.75, 1.0, 923.444444444);
  sgl::unsigned_fixed<10, 10> v1(float1);
  sgl::unsigned_fixed<10, 10> v2(float2);
  sgl::unsigned_fixed<17, 6>  v3(float1);
  sgl::unsigned_fixed<20, 20> v4(float2);
  auto                        result = to_double(v1 / v2);
  auto                        expected = to_double(decltype(v1 / v2)(float1 / float2));
  const double                delta = 0.001;
  CHECK_THAT(result, Catch::Matchers::WithinRel(expected, delta));
  auto res2 = to_double(v3 / v4);
  CHECK_THAT(res2, Catch::Matchers::WithinRel(expected, delta));
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

TEST_CASE("signed_fixed::integer()") {
  sgl::signed_fixed<20, 10> v1(20.75);
  REQUIRE(v1.integer() == 20);

  sgl::signed_fixed<20, 10> v2(123456.75);
  REQUIRE(v2.integer() == 123456);

  sgl::signed_fixed<20, 10> v3(-20.75);
  REQUIRE(v3.integer() == 0b11111111111111101011u);

  sgl::signed_fixed<20, 10> v4(-123456.75);
  REQUIRE(v4.integer() == 0b11100001110110111111u);
}

TEST_CASE("signed_fixed::fraction()") {
  sgl::signed_fixed<20, 10> v1(20.75);
  REQUIRE(v1.fraction() == 0b1100000000);

  sgl::signed_fixed<20, 10> v2(12345678.75);
  REQUIRE(v2.fraction() == 0b1100000000);

  sgl::signed_fixed<20, 10> v3(-20.75);
  REQUIRE(v3.fraction() == 0b0100000000);

  sgl::signed_fixed<20, 10> v4(-12345678.75);
  REQUIRE(v4.fraction() == 0b0100000000);
}

TEST_CASE("signed_fixed::is_negative") {
  GIVEN("A possibly floating point value") {
    auto f = GENERATE(1.0, 1.25, 5.5, 1.125);
    WHEN("converting it to signed_fixed") {
      THEN("negative numbers are negative") {
        sgl::signed_fixed<20, 10> neg(-f);
        REQUIRE(neg.is_negative());
        REQUIRE(not(-neg).is_negative());
      }
      THEN("positive numbers are not negative") {
        sgl::signed_fixed<20, 10> pos(f);
        REQUIRE(not pos.is_negative());
        REQUIRE((-pos).is_negative());
      }
    }
  }
}

TEST_CASE("operator-(signed_fixed)") {
  auto                      f = GENERATE(-0.25, -1.0, -25.75, 2.5, 13.875, 50.625);
  sgl::signed_fixed<20, 10> val(f);
  auto                      result = to_double(-val);
  REQUIRE(result == -f);
}

TEST_CASE("signed_fixed resize") {
  sgl::signed_fixed<2, 4> val{-1.0};
  REQUIRE(val.value() == 0b110000u);
  REQUIRE(sgl::sign_extended_value(val) == 0xF0u);
  auto resized = sgl::resize<4, 4>(val);
  REQUIRE(resized.value() == 0xF0u);
  sgl::signed_fixed<2, 4> val2{-1.0};
  REQUIRE(val.value() == 0b110000u);
  auto resized2 = sgl::resize<12, 12>(val);
  REQUIRE(resized2.value() == 0xFFF000u);
}

TEST_CASE("signed_fixed addition") {
  auto                      f1 = GENERATE(0.0, 5.5, 20.25, -20.625, 40.0, 12345.9874);
  auto                      f2 = GENERATE(-1.125, -2.0, 5.75, 123.575, -9876.5, 1.125);
  sgl::signed_fixed<20, 10> v1(f1);
  sgl::signed_fixed<20, 15> v2(f2);
  auto                      expected = to_double(decltype(v1 + v2)(f1 + f2));
  auto                      result = to_double(v1 + v2);
  REQUIRE_THAT(result, Catch::Matchers::WithinRel(expected, 0.01));
}

TEST_CASE("signed_fixed subtraction") {
  auto                      f1 = GENERATE(0.0, 5.5, 20.25, -20.625, 40.0, 12345.9874);
  auto                      f2 = GENERATE(-1.0, -2.0, 5.75, 123.575, -9876.5, 1.125);
  sgl::signed_fixed<20, 10> v1(f1);
  sgl::signed_fixed<15, 15> v2(f2);
  auto                      expected = to_double(decltype(v1 - v2)(f1 - f2));
  auto                      result = to_double(v1 - v2);
  REQUIRE_THAT(result, Catch::Matchers::WithinRel(expected, 0.01));
}

TEST_CASE("signed_fixed multiplication") {
  auto                      f1 = GENERATE(0.0, 5.5, 20.25, -20.625, 40.0, 345.9874);
  auto                      f2 = GENERATE(-1.0, -2.0, 5.75, 123.575, -76.5, 1.125);
  sgl::signed_fixed<20, 10> v1(f1);
  sgl::signed_fixed<20, 10> v2(f2);
  auto                      expected = to_double(decltype(v1 * v2)(f1 * f2));
  auto                      result = to_double(v1 * v2);
  REQUIRE_THAT(result, Catch::Matchers::WithinRel(expected, 0.01));
}

TEST_CASE("signed_fixed division") {
  auto                      float1 = GENERATE(-1.0, 2.25, 3.5, 21.5, 16.75, 7.4444444444);
  auto                      float2 = GENERATE(-2.25, 3.5, 21.5, -16.75, 1.0, -923.444444444);
  sgl::signed_fixed<12, 10> v1(float1);
  sgl::signed_fixed<12, 10> v2(float2);
  sgl::signed_fixed<17, 6>  v3(float2);
  sgl::signed_fixed<20, 20> v4(float1);
  auto                      result = to_double(v1 / v2);
  auto                      expected = to_double(decltype(v1 / v2)(float1 / float2));
  REQUIRE_THAT(result, Catch::Matchers::WithinRel(expected, 0.01));
  auto result2 = to_double(v3 / v4);
  auto expected2 = to_double(decltype(v3 / v4)(float2 / float1));
  REQUIRE_THAT(result2, Catch::Matchers::WithinRel(expected2, 0.01));
  if (expected < 0) {
    REQUIRE(result < 0);
    REQUIRE(result2 < 0);
  } else {
    REQUIRE(result >= 0);
    REQUIRE(result2 >= 0);
  }
}

TEST_CASE("to_signed") {
  auto f = GENERATE(1.0, 2.25, 3.5, 21.5, 16.75, 7.4444444444);
  GIVEN("an unsigned_fixed value") {
    sgl::unsigned_fixed<12, 10> v{f};
    WHEN("converting it to signed_fixed") {
      auto v2 = sgl::to_signed(v);
      THEN("the value does not change") { REQUIRE(v.value() == v2.value()); }
    }
  }
}

TEST_CASE("to_unsigned") {
  auto f = GENERATE(-1.0, 2.25, 3.5, 21.5, -16.75, 7.4444444444, 1234.23452);
  GIVEN("an signed_fixed value") {
    sgl::signed_fixed<12, 10> v{f};
    WHEN("converting it to unsigned_fixed") {
      auto v2 = sgl::to_unsigned(v);
      THEN("the value does not change") { REQUIRE(v.value() == v2.value()); }
    }
  }
}