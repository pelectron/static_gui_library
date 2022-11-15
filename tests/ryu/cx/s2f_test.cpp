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
#include <cmath>

#define EXPECT_S2F(a, b)                                                          \
  STATIC_REQUIRE([] {                                                             \
    float value{0};                                                              \
    return ((ryu::cx::s2f_n(b, (sizeof(b) <= 1) ? 0 : (sizeof(b) - 1), &value) == \
             ryu::status::success)) &&                                            \
           ((a) == value);                                                        \
  }())

TEST_CASE("cx::s2f_n") {
  SECTION("basic") {
    EXPECT_S2F(0.0f, "0");
    EXPECT_S2F(-0.0f, "-0");
    EXPECT_S2F(1.0f, "1");
    EXPECT_S2F(-1.0f, "-1");
    EXPECT_S2F(123456792.0f, "123456789");
    EXPECT_S2F(299792448.0f, "299792458");
  }

  SECTION("MinMax") {
    EXPECT_S2F(1e-45f, "1e-45");
    EXPECT_S2F(FLT_MIN, "1.1754944e-38");
    EXPECT_S2F(FLT_MAX, "3.4028235e+38");
  }

  SECTION("mantissa rounding overflow") {
    EXPECT_S2F(1.0f, "0.999999999");
    EXPECT_S2F(INFINITY, "3.4028236e+38");
    EXPECT_S2F(1.1754944e-38f, "1.17549430e-38"); // FLT_MIN
    EXPECT_S2F(1.1754944e-38f, "1.17549431e-38");
    EXPECT_S2F(1.1754944e-38f, "1.17549432e-38");
    EXPECT_S2F(1.1754944e-38f, "1.17549433e-38");
    EXPECT_S2F(1.1754944e-38f, "1.17549434e-38");
    EXPECT_S2F(1.1754944e-38f, "1.17549435e-38");
  }

  SECTION("trailing zeros") {
    EXPECT_S2F(26843550.0f, "26843549.5");
    EXPECT_S2F(50000004.0f, "50000002.5");
    EXPECT_S2F(99999992.0f, "99999989.5");
  }
}