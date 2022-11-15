// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#include "ryu/ryu_parse.hpp"

#include <catch2/catch.hpp>

#define EXPECT_S2D(a, b)                                                          \
  STATIC_REQUIRE([] {                                                             \
    double value{0};                                                              \
    return ((ryu::cx::s2d_n(b, (sizeof(b) <= 1) ? 0 : (sizeof(b) - 1), &value) == \
             ryu::Status::success)) &&                                            \
           ((a) == value);                                                        \
  }())

#define ASSERT_RETURN(inp, expected)                              \
  STATIC_REQUIRE([] {                                             \
    const auto size = (sizeof(inp) <= 1) ? 0 : (sizeof(inp) - 1); \
    double     value{0.0};                                        \
    return ryu::cx::s2d_n(inp, size, &value) == (expected);       \
  }())

TEST_CASE("cx::s2d_n") {
  SECTION("bad input") {
    ASSERT_RETURN("x", ryu::Status::malformed_input);
    ASSERT_RETURN("1..1", ryu::Status::malformed_input);
    ASSERT_RETURN("..", ryu::Status::malformed_input);
    ASSERT_RETURN("1..1", ryu::Status::malformed_input);
    ASSERT_RETURN("1ee1", ryu::Status::malformed_input);
    ASSERT_RETURN("1e.1", ryu::Status::malformed_input);
    ASSERT_RETURN("", ryu::Status::input_too_short);
    ASSERT_RETURN("123456789012345678", ryu::Status::input_too_long);
    ASSERT_RETURN("1e12345", ryu::Status::input_too_long);
  }

  SECTION("basic") {
    EXPECT_S2D(0.0, "0");
    EXPECT_S2D(-0.0, "-0");
    EXPECT_S2D(1.0, "1");
    EXPECT_S2D(2.0, "2");
    EXPECT_S2D(123456789.0, "123456789");
    EXPECT_S2D(123.456, "123.456");
    EXPECT_S2D(123.456, "123456e-3");
    EXPECT_S2D(123.456, "1234.56e-1");
    EXPECT_S2D(1.453, "1.453");
    EXPECT_S2D(1453.0, "1.453e+3");
    EXPECT_S2D(0.0, ".0");
    EXPECT_S2D(1.0, "1e0");
    EXPECT_S2D(1.0, "1E0");
    EXPECT_S2D(1.0, "000001.000000");
    EXPECT_S2D(0.2316419, "0.2316419");
  }

  SECTION("MinMax") {
    EXPECT_S2D(1.7976931348623157e308, "1.7976931348623157e308");
    EXPECT_S2D(5E-324, "5E-324");
  }

  SECTION("Mantissa rounding overflow") {
    // This results in binary mantissa that is all ones and requires rounding up
    // because it is closer to 1 than to the next smaller float. This is a
    // regression test that the mantissa overflow is handled correctly by
    // increasing the exponent.
    EXPECT_S2D(1.0, "0.99999999999999999");
    // This number overflows the mantissa *and* the IEEE exponent.
    EXPECT_S2D(INFINITY, "1.7976931348623159e308");
  }

  SECTION("underflow") {
    EXPECT_S2D(0.0, "2.4e-324");
    EXPECT_S2D(0.0, "1e-324");
    EXPECT_S2D(0.0, "9.99999e-325");
    // These are just about halfway between 0 and the smallest float.
    // The first is just below the halfway point, the second just above.
    EXPECT_S2D(0.0, "2.4703282292062327e-324");
    EXPECT_S2D(5e-324, "2.4703282292062328e-324");
  }

  SECTION("overflow") {
    EXPECT_S2D(INFINITY, "2e308");
    EXPECT_S2D(INFINITY, "1e309");
  }

  SECTION("table size denormal") { EXPECT_S2D(5e-324, "4.9406564584124654e-324"); }

  SECTION("Issue157") { EXPECT_S2D(1.2999999999999999E+154, "1.2999999999999999E+154"); }

  SECTION("Issue173") {
    // Denormal boundary
    EXPECT_S2D(2.2250738585072012e-308, "2.2250738585072012e-308");
    EXPECT_S2D(2.2250738585072013e-308, "2.2250738585072013e-308");
    EXPECT_S2D(2.2250738585072014e-308, "2.2250738585072014e-308");
  }
}
