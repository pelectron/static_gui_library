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
#include "ryu/common.hpp"
#include "ryu/d2s_full_table.hpp"
#include "ryu/d2s_intrinsics.hpp"
#include "ryu/d2s_small_table.hpp"

#include <cinttypes>
#include <cmath>

TEST_CASE("d2s table") {
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
