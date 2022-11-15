// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#include "cx_test_util.hpp"
#include "ryu/ryu.hpp"

#include <catch2/catch.hpp>
#include <cmath>
#include <gcem.hpp>

static constexpr bool test_f2s(const char* expected, double d) {
  char buf[512]{0};
  auto size = ryu::cx::f2s_buffered_n(d, buf);
  return cmp(expected, buf, size);
}

// with this macro, compile time string formatting is enforced, i.e. every CX_ASSERT_F2S statement
// is executed at compile time.
#define CX_ASSERT_F2S(expected, d) STATIC_REQUIRE(test_f2s(expected, d))

TEST_CASE("cx::f2s_buffered", "[ryu][f2s][compile_time") {
  SECTION("Basic") {
    CX_ASSERT_F2S("0E0", 0.0);
    // CX_ASSERT_F2S("-0E0", -0.0); cant differentiate positive and negative zero in cx::to_bits
    CX_ASSERT_F2S("1E0", 1.0);
    CX_ASSERT_F2S("-1E0", -1.0);
    REQUIRE(test_f2s("NaN", NAN)); // cannot be constexpr
    CX_ASSERT_F2S("Infinity", INFINITY);
    CX_ASSERT_F2S("-Infinity", -INFINITY);
  }

  SECTION("SwitchToSubnormal") { CX_ASSERT_F2S("1.1754944E-38", 1.1754944E-38f); }

  SECTION("MinAndMax") {
    CX_ASSERT_F2S("3.4028235E38", 3.4028235E38f);
    CX_ASSERT_F2S("1E-45", 1E-45f);
  }

  // Check that we return the exact boundary if it is the shortest
  // representation, but only if the original floating point number is even.
  SECTION("BoundaryRoundEven") {
    CX_ASSERT_F2S("3.355445E7", 3.355445E7f);
    CX_ASSERT_F2S("9E9", 8.999999E9f);
    CX_ASSERT_F2S("3.436672E10", 3.4366717E10f);
  }

  // If the exact value is exactly halfway between two shortest representations,
  // then we round to even. It seems like this only makes a difference if the
  // last two digits are ...2|5 or ...7|5, and we cut off the 5.
  SECTION("ExactValueRoundEven") {
    CX_ASSERT_F2S("3.0540412E5", 3.0540412E5f);
    CX_ASSERT_F2S("8.0990312E3", 8.0990312E3f);
  }

  SECTION("LotsOfTrailingZeros") {
    // Pattern for the first test: 00111001100000000000000000000000
    CX_ASSERT_F2S("2.4414062E-4", 2.4414062E-4f);
    CX_ASSERT_F2S("2.4414062E-3", 2.4414062E-3f);
    CX_ASSERT_F2S("4.3945312E-3", 4.3945312E-3f);
    CX_ASSERT_F2S("6.3476562E-3", 6.3476562E-3f);
  }

  SECTION("Regression") {
    CX_ASSERT_F2S("4.7223665E21", 4.7223665E21f);
    CX_ASSERT_F2S("8.388608E6", 8388608.0f);
    CX_ASSERT_F2S("1.6777216E7", 1.6777216E7f);
    CX_ASSERT_F2S("3.3554436E7", 3.3554436E7f);
    CX_ASSERT_F2S("6.7131496E7", 6.7131496E7f);
    CX_ASSERT_F2S("1.9310392E-38", 1.9310392E-38f);
    CX_ASSERT_F2S("-2.47E-43", -2.47E-43f);
    CX_ASSERT_F2S("1.993244E-38", 1.993244E-38f);
    CX_ASSERT_F2S("4.1039004E3", 4103.9003f);
    CX_ASSERT_F2S("5.3399997E9", 5.3399997E9f);
    CX_ASSERT_F2S("6.0898E-39", 6.0898E-39f);
    CX_ASSERT_F2S("1.0310042E-3", 0.0010310042f);
    CX_ASSERT_F2S("2.882326E17", 2.8823261E17f);
#ifndef _WIN32
    // MSVC rounds this up to the next higher floating point number
    CX_ASSERT_F2S("7.038531E-26", 7.038531E-26f);
#else
    CX_ASSERT_F2S("7.038531E-26", 7.0385309E-26f);
#endif
    CX_ASSERT_F2S("9.223404E17", 9.2234038E17f);
    CX_ASSERT_F2S("6.710887E7", 6.7108872E7f);
    CX_ASSERT_F2S("1E-44", 1.0E-44f);
    CX_ASSERT_F2S("2.816025E14", 2.816025E14f);
    CX_ASSERT_F2S("9.223372E18", 9.223372E18f);
    CX_ASSERT_F2S("1.5846086E29", 1.5846085E29f);
    CX_ASSERT_F2S("1.1811161E19", 1.1811161E19f);
    CX_ASSERT_F2S("5.368709E18", 5.368709E18f);
    CX_ASSERT_F2S("4.6143166E18", 4.6143165E18f);
    CX_ASSERT_F2S("7.812537E-3", 0.007812537f);
    CX_ASSERT_F2S("1E-45", 1.4E-45f);
    CX_ASSERT_F2S("1.18697725E20", 1.18697724E20f);
    CX_ASSERT_F2S("1.00014165E-36", 1.00014165E-36f);
    CX_ASSERT_F2S("2E2", 200.0f);
    CX_ASSERT_F2S("3.3554432E7", 3.3554432E7f);
  }

  SECTION("LooksLikePow5") {
    // These numbers have a mantissa that is the largest power of 5 that fits,
    // and an exponent that causes the computation for q to result in 10, which is a corner
    // case for Ryu.
    CX_ASSERT_F2S("6.7108864E17", 6.7108864E17f);
    CX_ASSERT_F2S("1.3421773E18", 1.3421773E18f);
    CX_ASSERT_F2S("2.6843546E18", 2.6843546E18f);
  }

  SECTION("OutputLength") {
    CX_ASSERT_F2S("1E0", 1.0f); // already tested in Basic
    CX_ASSERT_F2S("1.2E0", 1.2f);
    CX_ASSERT_F2S("1.23E0", 1.23f);
    CX_ASSERT_F2S("1.234E0", 1.234f);
    CX_ASSERT_F2S("1.2345E0", 1.2345f);
    CX_ASSERT_F2S("1.23456E0", 1.23456f);
    CX_ASSERT_F2S("1.234567E0", 1.234567f);
    CX_ASSERT_F2S("1.2345678E0", 1.2345678f);
    CX_ASSERT_F2S("1.23456735E-36", 1.23456735E-36f);
  }
}