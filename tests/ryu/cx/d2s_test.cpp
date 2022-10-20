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
#include "gcem.hpp"
#include "ryu/ryu.hpp"

#include <catch2/catch.hpp>
#include <cmath>
#include <cstring>


static constexpr bool test_d2s(const char* expected, double d) {
  char buf[512]{0};
  auto size = ryu::cx::d2s_buffered_n(d, buf);
  return cmp(expected, buf, size);
}

TEST_CASE("cx::d2s_buffered_n", "[ryu][d2s][compile_time") {
  SECTION("Basic") {
    STATIC_REQUIRE(test_d2s("0E0", 0.0));
    // STATIC_REQUIRE(test_d2s("-0E0", -0.0)); cant differentiate between -0 and 0 in cx::to_bits
    STATIC_REQUIRE(test_d2s("1E0", 1.0));
    STATIC_REQUIRE(test_d2s("-1E0", -1.0));
    // STATIC_REQUIRE(test_d2s("NaN", NAN));
    STATIC_REQUIRE(test_d2s("Infinity", INFINITY));
    STATIC_REQUIRE(test_d2s("-Infinity", -INFINITY));
  }

  SECTION("SwitchToSubnormal") {
    STATIC_REQUIRE(test_d2s("2.2250738585072014E-308", 2.2250738585072014E-308));
  }

  SECTION("MinAndMax") {
    STATIC_REQUIRE(test_d2s("1.7976931348623157E308", 1.7976931348623157E308));
    // TODO: broken test
    // STATIC_REQUIRE(test_d2s("5E-324", 4.9406564584124654E-324));
  }

  SECTION("LotsOfTrailingZeros") {
    STATIC_REQUIRE(test_d2s("2.9802322387695312E-8", 2.98023223876953125E-8));
  }

  SECTION("Regression") {
    STATIC_REQUIRE(test_d2s("-2.109808898695963E16", -2.109808898695963E16));
    // TODO: broken tests
    // STATIC_REQUIRE(test_d2s("4.940656E-318", 4.940656E-318));
    // STATIC_REQUIRE(test_d2s("1.18575755E-316", 1.18575755E-316));
    // STATIC_REQUIRE(test_d2s("2.989102097996E-312", 2.989102097996E-312));
    STATIC_REQUIRE(test_d2s("9.0608011534336E15", 9.0608011534336E15));
    STATIC_REQUIRE(test_d2s("4.708356024711512E18", 4.708356024711512E18));
    STATIC_REQUIRE(test_d2s("9.409340012568248E18", 9.409340012568248E18));
    STATIC_REQUIRE(test_d2s("1.2345678E0", 1.2345678));
  }

  SECTION("LooksLikePow5") {
    // These numbers have a mantissa that is a multiple of the largest power of 5 that fits,
    // and an exponent that causes the computation for q to result in 22, which is a corner
    // case for Ryu.
    STATIC_REQUIRE(test_d2s("5.764607523034235E39", 5.764607523034235E39));
    STATIC_REQUIRE(test_d2s("1.152921504606847E40", 1.152921504606847E40));
    STATIC_REQUIRE(test_d2s("2.305843009213694E40", 2.305843009213694E40));
  }

  SECTION("OutputLength") {
    STATIC_REQUIRE(test_d2s("1E0", 1)); // already tested in Basic
    STATIC_REQUIRE(test_d2s("1.2E0", 1.2));
    STATIC_REQUIRE(test_d2s("1.23E0", 1.23));
    STATIC_REQUIRE(test_d2s("1.234E0", 1.234));
    STATIC_REQUIRE(test_d2s("1.2345E0", 1.2345));
    STATIC_REQUIRE(test_d2s("1.23456E0", 1.23456));
    STATIC_REQUIRE(test_d2s("1.234567E0", 1.234567));
    STATIC_REQUIRE(test_d2s("1.2345678E0", 1.2345678)); // already tested in Regression
    STATIC_REQUIRE(test_d2s("1.23456789E0", 1.23456789));
    STATIC_REQUIRE(test_d2s("1.234567895E0", 1.234567895)); // 1.234567890 would be trimmed
    STATIC_REQUIRE(test_d2s("1.2345678901E0", 1.2345678901));
    STATIC_REQUIRE(test_d2s("1.23456789012E0", 1.23456789012));
    STATIC_REQUIRE(test_d2s("1.234567890123E0", 1.234567890123));
    STATIC_REQUIRE(test_d2s("1.2345678901234E0", 1.2345678901234));
    STATIC_REQUIRE(test_d2s("1.23456789012345E0", 1.23456789012345));
    STATIC_REQUIRE(test_d2s("1.234567890123456E0", 1.234567890123456));
    STATIC_REQUIRE(test_d2s("1.2345678901234567E0", 1.2345678901234567));

    // Test 32-bit chunking
    STATIC_REQUIRE(test_d2s("4.294967294E0", 4.294967294)); // 2^32 - 2
    STATIC_REQUIRE(test_d2s("4.294967295E0", 4.294967295)); // 2^32 - 1
    STATIC_REQUIRE(test_d2s("4.294967296E0", 4.294967296)); // 2^32
    STATIC_REQUIRE(test_d2s("4.294967297E0", 4.294967297)); // 2^32 + 1
    STATIC_REQUIRE(test_d2s("4.294967298E0", 4.294967298)); // 2^32 + 2
  }

  // Test min, max shift values in shiftright128
  SECTION("MinMaxShift") {
    // TODO: #3 ryu investigate error when maxMAntissa in double formatting
    // const uint64_t maxMantissa = ((uint64_t)1 << 53) - 1;

    // 32-bit opt-size=0:  49 <= dist <= 50
    // 32-bit opt-size=1:  30 <= dist <= 50
    // 64-bit opt-size=0:  50 <= dist <= 50
    // 64-bit opt-size=1:  30 <= dist <= 50
    STATIC_REQUIRE(test_d2s("1.7800590868057611E-307", ieeeParts2Double(false, 4, 0)));
    // 32-bit opt-size=0:  49 <= dist <= 49
    // 32-bit opt-size=1:  28 <= dist <= 49
    // 64-bit opt-size=0:  50 <= dist <= 50
    // 64-bit opt-size=1:  28 <= dist <= 50
    // TODO: #3
    // STATIC_REQUIRE(test_d2s("2.8480945388892175E-306", ieeeParts2Double(false, 6, maxMantissa)));
    // 32-bit opt-size=0:  52 <= dist <= 53
    // 32-bit opt-size=1:   2 <= dist <= 53
    // 64-bit opt-size=0:  53 <= dist <= 53
    // 64-bit opt-size=1:   2 <= dist <= 53
    STATIC_REQUIRE(test_d2s("2.446494580089078E-296", ieeeParts2Double(false, 41, 0)));
    // 32-bit opt-size=0:  52 <= dist <= 52
    // 32-bit opt-size=1:   2 <= dist <= 52
    // 64-bit opt-size=0:  53 <= dist <= 53
    // 64-bit opt-size=1:   2 <= dist <= 53
    // TODO: #3
    // STATIC_REQUIRE(test_d2s("4.8929891601781557E-296", ieeeParts2Double(false, 40,
    // maxMantissa)));

    // 32-bit opt-size=0:  57 <= dist <= 58
    // 32-bit opt-size=1:  57 <= dist <= 58
    // 64-bit opt-size=0:  58 <= dist <= 58
    // 64-bit opt-size=1:  58 <= dist <= 58
    STATIC_REQUIRE(test_d2s("1.8014398509481984E16", ieeeParts2Double(false, 1077, 0)));
    // 32-bit opt-size=0:  57 <= dist <= 57
    // 32-bit opt-size=1:  57 <= dist <= 57
    // 64-bit opt-size=0:  58 <= dist <= 58
    // 64-bit opt-size=1:  58 <= dist <= 58
    // TODO: #3
    // STATIC_REQUIRE(test_d2s("3.6028797018963964E16", ieeeParts2Double(false, 1076,
    // maxMantissa)));
    // 32-bit opt-size=0:  51 <= dist <= 52
    // 32-bit opt-size=1:  51 <= dist <= 59
    // 64-bit opt-size=0:  52 <= dist <= 52
    // 64-bit opt-size=1:  52 <= dist <= 59
    STATIC_REQUIRE(test_d2s("2.900835519859558E-216", ieeeParts2Double(false, 307, 0)));
    // 32-bit opt-size=0:  51 <= dist <= 51
    // 32-bit opt-size=1:  51 <= dist <= 59
    // 64-bit opt-size=0:  52 <= dist <= 52
    // 64-bit opt-size=1:  52 <= dist <= 59
    // TODO: #3
    // STATIC_REQUIRE(test_d2s("5.801671039719115E-216", ieeeParts2Double(false, 306,
    // maxMantissa)));

    // https://github.com/ulfjack/ryu/commit/19e44d16d80236f5de25800f56d82606d1be00b9#commitcomment-30146483
    // 32-bit opt-size=0:  49 <= dist <= 49
    // 32-bit opt-size=1:  44 <= dist <= 49
    // 64-bit opt-size=0:  50 <= dist <= 50
    // 64-bit opt-size=1:  44 <= dist <= 50
    // TODO: #3
    // STATIC_REQUIRE(test_d2s("3.196104012172126E-27", ieeeParts2Double(false, 934,
    // 0x000FA7161A4D6E0Cu)));
  }

  SECTION("SmallIntegers") {
    STATIC_REQUIRE(test_d2s("9.007199254740991E15", 9007199254740991.0)); // 2^53-1
    STATIC_REQUIRE(test_d2s("9.007199254740992E15", 9007199254740992.0)); // 2^53

    STATIC_REQUIRE(test_d2s("1E0", 1.0e+0));
    STATIC_REQUIRE(test_d2s("1.2E1", 1.2e+1));
    STATIC_REQUIRE(test_d2s("1.23E2", 1.23e+2));
    STATIC_REQUIRE(test_d2s("1.234E3", 1.234e+3));
    STATIC_REQUIRE(test_d2s("1.2345E4", 1.2345e+4));
    STATIC_REQUIRE(test_d2s("1.23456E5", 1.23456e+5));
    STATIC_REQUIRE(test_d2s("1.234567E6", 1.234567e+6));
    STATIC_REQUIRE(test_d2s("1.2345678E7", 1.2345678e+7));
    STATIC_REQUIRE(test_d2s("1.23456789E8", 1.23456789e+8));
    STATIC_REQUIRE(test_d2s("1.23456789E9", 1.23456789e+9));
    STATIC_REQUIRE(test_d2s("1.234567895E9", 1.234567895e+9));
    STATIC_REQUIRE(test_d2s("1.2345678901E10", 1.2345678901e+10));
    STATIC_REQUIRE(test_d2s("1.23456789012E11", 1.23456789012e+11));
    STATIC_REQUIRE(test_d2s("1.234567890123E12", 1.234567890123e+12));
    STATIC_REQUIRE(test_d2s("1.2345678901234E13", 1.2345678901234e+13));
    STATIC_REQUIRE(test_d2s("1.23456789012345E14", 1.23456789012345e+14));
    STATIC_REQUIRE(test_d2s("1.234567890123456E15", 1.234567890123456e+15));

    // 10^i
    STATIC_REQUIRE(test_d2s("1E0", 1.0e+0));
    STATIC_REQUIRE(test_d2s("1E1", 1.0e+1));
    STATIC_REQUIRE(test_d2s("1E2", 1.0e+2));
    STATIC_REQUIRE(test_d2s("1E3", 1.0e+3));
    STATIC_REQUIRE(test_d2s("1E4", 1.0e+4));
    STATIC_REQUIRE(test_d2s("1E5", 1.0e+5));
    STATIC_REQUIRE(test_d2s("1E6", 1.0e+6));
    STATIC_REQUIRE(test_d2s("1E7", 1.0e+7));
    STATIC_REQUIRE(test_d2s("1E8", 1.0e+8));
    STATIC_REQUIRE(test_d2s("1E9", 1.0e+9));
    STATIC_REQUIRE(test_d2s("1E10", 1.0e+10));
    STATIC_REQUIRE(test_d2s("1E11", 1.0e+11));
    STATIC_REQUIRE(test_d2s("1E12", 1.0e+12));
    STATIC_REQUIRE(test_d2s("1E13", 1.0e+13));
    STATIC_REQUIRE(test_d2s("1E14", 1.0e+14));
    STATIC_REQUIRE(test_d2s("1E15", 1.0e+15));

    // 10^15 + 10^i
    STATIC_REQUIRE(test_d2s("1.000000000000001E15", 1.0e+15 + 1.0e+0));
    STATIC_REQUIRE(test_d2s("1.00000000000001E15", 1.0e+15 + 1.0e+1));
    STATIC_REQUIRE(test_d2s("1.0000000000001E15", 1.0e+15 + 1.0e+2));
    STATIC_REQUIRE(test_d2s("1.000000000001E15", 1.0e+15 + 1.0e+3));
    STATIC_REQUIRE(test_d2s("1.00000000001E15", 1.0e+15 + 1.0e+4));
    STATIC_REQUIRE(test_d2s("1.0000000001E15", 1.0e+15 + 1.0e+5));
    STATIC_REQUIRE(test_d2s("1.000000001E15", 1.0e+15 + 1.0e+6));
    STATIC_REQUIRE(test_d2s("1.00000001E15", 1.0e+15 + 1.0e+7));
    STATIC_REQUIRE(test_d2s("1.0000001E15", 1.0e+15 + 1.0e+8));
    STATIC_REQUIRE(test_d2s("1.000001E15", 1.0e+15 + 1.0e+9));
    STATIC_REQUIRE(test_d2s("1.00001E15", 1.0e+15 + 1.0e+10));
    STATIC_REQUIRE(test_d2s("1.0001E15", 1.0e+15 + 1.0e+11));
    STATIC_REQUIRE(test_d2s("1.001E15", 1.0e+15 + 1.0e+12));
    STATIC_REQUIRE(test_d2s("1.01E15", 1.0e+15 + 1.0e+13));
    STATIC_REQUIRE(test_d2s("1.1E15", 1.0e+15 + 1.0e+14));

    // Largest power of 2 <= 10^(i+1)
    STATIC_REQUIRE(test_d2s("8E0", 8.0));
    STATIC_REQUIRE(test_d2s("6.4E1", 64.0));
    STATIC_REQUIRE(test_d2s("5.12E2", 512.0));
    STATIC_REQUIRE(test_d2s("8.192E3", 8192.0));
    STATIC_REQUIRE(test_d2s("6.5536E4", 65536.0));
    STATIC_REQUIRE(test_d2s("5.24288E5", 524288.0));
    STATIC_REQUIRE(test_d2s("8.388608E6", 8388608.0));
    STATIC_REQUIRE(test_d2s("6.7108864E7", 67108864.0));
    STATIC_REQUIRE(test_d2s("5.36870912E8", 536870912.0));
    STATIC_REQUIRE(test_d2s("8.589934592E9", 8589934592.0));
    STATIC_REQUIRE(test_d2s("6.8719476736E10", 68719476736.0));
    STATIC_REQUIRE(test_d2s("5.49755813888E11", 549755813888.0));
    STATIC_REQUIRE(test_d2s("8.796093022208E12", 8796093022208.0));
    STATIC_REQUIRE(test_d2s("7.0368744177664E13", 70368744177664.0));
    STATIC_REQUIRE(test_d2s("5.62949953421312E14", 562949953421312.0));
    STATIC_REQUIRE(test_d2s("9.007199254740992E15", 9007199254740992.0));

    // 1000 * (Largest power of 2 <= 10^(i+1))
    STATIC_REQUIRE(test_d2s("8E3", 8.0e+3));
    STATIC_REQUIRE(test_d2s("6.4E4", 64.0e+3));
    STATIC_REQUIRE(test_d2s("5.12E5", 512.0e+3));
    STATIC_REQUIRE(test_d2s("8.192E6", 8192.0e+3));
    STATIC_REQUIRE(test_d2s("6.5536E7", 65536.0e+3));
    STATIC_REQUIRE(test_d2s("5.24288E8", 524288.0e+3));
    STATIC_REQUIRE(test_d2s("8.388608E9", 8388608.0e+3));
    STATIC_REQUIRE(test_d2s("6.7108864E10", 67108864.0e+3));
    STATIC_REQUIRE(test_d2s("5.36870912E11", 536870912.0e+3));
    STATIC_REQUIRE(test_d2s("8.589934592E12", 8589934592.0e+3));
    STATIC_REQUIRE(test_d2s("6.8719476736E13", 68719476736.0e+3));
    STATIC_REQUIRE(test_d2s("5.49755813888E14", 549755813888.0e+3));
    STATIC_REQUIRE(test_d2s("8.796093022208E15", 8796093022208.0e+3));
  }
}