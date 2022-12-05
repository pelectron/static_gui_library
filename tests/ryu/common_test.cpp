// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//

#include "ryu/common.hpp"

#include <catch2/catch.hpp>
#include <cmath>
#include <cstring>


#define EXPECT_EQ(a, b)               REQUIRE((a) == (b))
#define EXPECT_STREQ(literal, buffer) REQUIRE(strncmp(literal, buffer, sizeof(literal)) == 0);

using namespace ryu::detail;

TEST_CASE("CommonTest", "[ryu][common]") {
  SECTION("decimalLength9") {
    EXPECT_EQ(1u, decimalLength9(0));
    EXPECT_EQ(1u, decimalLength9(1));
    EXPECT_EQ(1u, decimalLength9(9));
    EXPECT_EQ(2u, decimalLength9(10));
    EXPECT_EQ(2u, decimalLength9(99));
    EXPECT_EQ(3u, decimalLength9(100));
    EXPECT_EQ(3u, decimalLength9(999));
    EXPECT_EQ(9u, decimalLength9(999999999));
  }

  SECTION("ceil_log2pow5") {
    EXPECT_EQ(1, ceil_log2pow5(0));
    EXPECT_EQ(3, ceil_log2pow5(1));
    EXPECT_EQ(5, ceil_log2pow5(2));
    EXPECT_EQ(7, ceil_log2pow5(3));
    EXPECT_EQ(10, ceil_log2pow5(4));
    EXPECT_EQ(8192, ceil_log2pow5(3528));
  }

  SECTION("log10Pow2") {
    EXPECT_EQ(0u, log10Pow2(0));
    EXPECT_EQ(0u, log10Pow2(1));
    EXPECT_EQ(0u, log10Pow2(2));
    EXPECT_EQ(0u, log10Pow2(3));
    EXPECT_EQ(1u, log10Pow2(4));
    EXPECT_EQ(496u, log10Pow2(1650));
  }

  SECTION("log10Pow5") {
    EXPECT_EQ(0u, log10Pow5(0));
    EXPECT_EQ(0u, log10Pow5(1));
    EXPECT_EQ(1u, log10Pow5(2));
    EXPECT_EQ(2u, log10Pow5(3));
    EXPECT_EQ(2u, log10Pow5(4));
    EXPECT_EQ(1831u, log10Pow5(2620));
  }

  SECTION("copy_special_str") {
    char buffer[100];
    memset(buffer, '\0', 100);
    EXPECT_EQ(3, copy_special_str(buffer, false, false, true));
    EXPECT_STREQ("NaN", buffer);

    memset(buffer, '\0', 100);
    EXPECT_EQ(8, copy_special_str(buffer, false, true, false));
    EXPECT_STREQ("Infinity", buffer);

    memset(buffer, '\0', 100);
    EXPECT_EQ(9, copy_special_str(buffer, true, true, false));
    EXPECT_STREQ("-Infinity", buffer);

    memset(buffer, '\0', 100);
    EXPECT_EQ(3, copy_special_str(buffer, false, false, false));
    EXPECT_STREQ("0E0", buffer);

    memset(buffer, '\0', 100);
    EXPECT_EQ(4, copy_special_str(buffer, true, false, false));
    EXPECT_STREQ("-0E0", buffer);
  }

  SECTION("float_to_bits") {
    EXPECT_EQ(0u, ryu::to_bits(0.0f));
    EXPECT_EQ(0x40490fda, ryu::to_bits(3.1415926f));
    EXPECT_EQ(0x56E0910C, ryu::to_bits(123456789101112.0f));
    EXPECT_EQ(0x7FC00000, ryu::to_bits(std::numeric_limits<float>::quiet_NaN()));
    EXPECT_EQ(0x7F800000, ryu::to_bits(std::numeric_limits<float>::infinity()));
    EXPECT_EQ(0xFF800000, ryu::to_bits(-std::numeric_limits<float>::infinity()));
  }

  SECTION("double_to_bits") {
    EXPECT_EQ(0ull, ryu::to_bits(0.0));
    EXPECT_EQ(0x400921FB54442D18, ryu::to_bits(3.1415926535897932384626433));
    EXPECT_EQ(0x42DC122183CE8E00, ryu::to_bits(123456789101112.0));
    EXPECT_EQ(0x7FF8000000000000, ryu::to_bits(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_EQ(0x7FF0000000000000, ryu::to_bits(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(0xFFF0000000000000, ryu::to_bits(-std::numeric_limits<double>::infinity()));
  }
  SECTION("cx float to_bits") {
    EXPECT_EQ(0u, ryu::cx::to_bits(0.0f));
    EXPECT_EQ(0x40490fda, ryu::cx::to_bits(3.1415926f));
    EXPECT_EQ(0x3C4A42AF, ryu::cx::to_bits(0.012345f));
    EXPECT_EQ(0x56E0910C, ryu::cx::to_bits(123456789101112.0f));
    EXPECT_EQ(0x7FC00000, ryu::cx::to_bits(std::numeric_limits<float>::quiet_NaN()));
    EXPECT_EQ(0x7F800000, ryu::cx::to_bits(std::numeric_limits<float>::infinity()));
    EXPECT_EQ(0xFF800000, ryu::cx::to_bits(-std::numeric_limits<float>::infinity()));
  }

  SECTION("cx double to_bits") {
    EXPECT_EQ(0ull, ryu::cx::to_bits(0.0));
    EXPECT_EQ(0x400921FB54442D18, ryu::cx::to_bits(3.1415926535897932384626433));
    EXPECT_EQ(0x3F894855DA272863, ryu::cx::to_bits(0.012345f));
    EXPECT_EQ(0x42DC122183CE8E00, ryu::cx::to_bits(123456789101112.0));
    EXPECT_EQ(0x7FF8000000000000, ryu::cx::to_bits(std::numeric_limits<double>::quiet_NaN()));
    EXPECT_EQ(0x7FF0000000000000, ryu::cx::to_bits(std::numeric_limits<double>::infinity()));
    EXPECT_EQ(0xFFF0000000000000, ryu::cx::to_bits(-std::numeric_limits<double>::infinity()));
  }
}
