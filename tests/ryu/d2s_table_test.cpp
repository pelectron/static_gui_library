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
#include "ryu/d2s_full_table.hpp"
#include "ryu/d2s_intrinsics.hpp"
#include "ryu/d2s_small_table.hpp"

#include <catch2/catch.hpp>
#include <cinttypes>
#include <cmath>


TEST_CASE("d2s table", "[ryu]") {
  SECTION("double_computePow5") {
    for (int i = 0; i < 326; i++) {
      uint64_t m[2];
      ryu::detail::double_computePow5(i, m);
      REQUIRE(m[0] == ryu::detail::DOUBLE_POW5_SPLIT[i][0]);
      REQUIRE(m[1] == ryu::detail::DOUBLE_POW5_SPLIT[i][1]);
    }
  }

  SECTION("double_computeInvPow5") {
    for (int i = 0; i < 292; i++) {
      uint64_t m[2];
      ryu::detail::double_computeInvPow5(i, m);
      REQUIRE(m[0] == ryu::detail::DOUBLE_POW5_INV_SPLIT[i][0]);
      REQUIRE(m[1] == ryu::detail::DOUBLE_POW5_INV_SPLIT[i][1]);
    }
  }
}
