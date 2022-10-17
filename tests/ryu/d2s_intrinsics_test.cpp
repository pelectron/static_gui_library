// Copyright 2018 Ulf Adams
//
// The contents of this file may be used under the terms of the Apache License,
// Version 2.0.
//
//    (See accompanying file LICENSE-Apache or copy at
//     http://www.apache.org/licenses/LICENSE-2.0)
//
// Alternatively, the contents of this file may be used under the terms of
// the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE-Boost or copy at
//     https://www.boost.org/LICENSE_1_0.txt)
//
// Unless required by applicable law or agreed to in writing, this software
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.

#include "catch2/catch.hpp"
#include "ryu/d2s_intrinsics.hpp"

#include <cmath>

TEST_CASE("D2sIntrinsicsTest") {
  SECTION("mod1e9") {
    REQUIRE(0u == ryu::detail::mod1e9(0));
    REQUIRE(1u == ryu::detail::mod1e9(1));
    REQUIRE(2u == ryu::detail::mod1e9(2));
    REQUIRE(10u == ryu::detail::mod1e9(10));
    REQUIRE(100u == ryu::detail::mod1e9(100));
    REQUIRE(1000u == ryu::detail::mod1e9(1000));
    REQUIRE(10000u == ryu::detail::mod1e9(10000));
    REQUIRE(100000u == ryu::detail::mod1e9(100000));
    REQUIRE(1000000u == ryu::detail::mod1e9(1000000));
    REQUIRE(10000000u == ryu::detail::mod1e9(10000000));
    REQUIRE(100000000u == ryu::detail::mod1e9(100000000));
    REQUIRE(0u == ryu::detail::mod1e9(1000000000));
    REQUIRE(0u == ryu::detail::mod1e9(2000000000));
    REQUIRE(1u == ryu::detail::mod1e9(1000000001));
    REQUIRE(1234u == ryu::detail::mod1e9(1000001234));
    REQUIRE(123456789u == ryu::detail::mod1e9(12345123456789ull));
    REQUIRE(123456789u == ryu::detail::mod1e9(123456789123456789ull));
  }

  SECTION("shiftRight128") {
    REQUIRE(0x100000000ull == ryu::detail::shiftright128(0x1ull, 0x1ull, 32));
  }

  SECTION("pow5Factor") {
    REQUIRE(0u == ryu::detail::pow5Factor(1ull));
    REQUIRE(0u == ryu::detail::pow5Factor(2ull));
    REQUIRE(0u == ryu::detail::pow5Factor(3ull));
    REQUIRE(0u == ryu::detail::pow5Factor(4ull));
    REQUIRE(1u == ryu::detail::pow5Factor(5ull));
    REQUIRE(0u == ryu::detail::pow5Factor(6ull));
    REQUIRE(0u == ryu::detail::pow5Factor(7ull));
    REQUIRE(0u == ryu::detail::pow5Factor(8ull));
    REQUIRE(0u == ryu::detail::pow5Factor(9ull));
    REQUIRE(1u == ryu::detail::pow5Factor(10ull));

    REQUIRE(0u == ryu::detail::pow5Factor(12ull));
    REQUIRE(0u == ryu::detail::pow5Factor(14ull));
    REQUIRE(0u == ryu::detail::pow5Factor(16ull));
    REQUIRE(0u == ryu::detail::pow5Factor(18ull));
    REQUIRE(1u == ryu::detail::pow5Factor(20ull));

    REQUIRE(2u == ryu::detail::pow5Factor(5 * 5ull));
    REQUIRE(3u == ryu::detail::pow5Factor(5 * 5 * 5ull));
    REQUIRE(4u == ryu::detail::pow5Factor(5 * 5 * 5 * 5ull));
    REQUIRE(5u == ryu::detail::pow5Factor(5 * 5 * 5 * 5 * 5ull));
    REQUIRE(6u == ryu::detail::pow5Factor(5 * 5 * 5 * 5 * 5 * 5ull));
    REQUIRE(7u == ryu::detail::pow5Factor(5 * 5 * 5 * 5 * 5 * 5 * 5ull));
    REQUIRE(8u == ryu::detail::pow5Factor(5 * 5 * 5 * 5 * 5 * 5 * 5 * 5ull));
    REQUIRE(9u == ryu::detail::pow5Factor(5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5ull));
    REQUIRE(10u == ryu::detail::pow5Factor(5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5 * 5ull));

    REQUIRE(0u == ryu::detail::pow5Factor(42ull));
    REQUIRE(1u == ryu::detail::pow5Factor(42 * 5ull));
    REQUIRE(2u == ryu::detail::pow5Factor(42 * 5 * 5ull));
    REQUIRE(3u == ryu::detail::pow5Factor(42 * 5 * 5 * 5ull));
    REQUIRE(4u == ryu::detail::pow5Factor(42 * 5 * 5 * 5 * 5ull));
    REQUIRE(5u == ryu::detail::pow5Factor(42 * 5 * 5 * 5 * 5 * 5ull));

    REQUIRE(27u ==
            ryu::detail::pow5Factor(7450580596923828125ull)); // 5^27, largest power of 5 < 2^64.
    REQUIRE(1u == ryu::detail::pow5Factor(
                      18446744073709551615ull)); // 2^64 - 1, largest multiple of 5 < 2^64.
    REQUIRE(0u == ryu::detail::pow5Factor(
                      18446744073709551614ull)); // 2^64 - 2, largest non-multiple of 5 < 2^64.
  }
}