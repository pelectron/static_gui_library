//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#include "sgl/cx_arg.hpp"

#include <catch2/catch.hpp>

using namespace sgl::cx_arg_literals;

template <typename Float>
constexpr bool epsilon_equal(Float f1, Float f2, Float epsilon = 0.000001) {
  Float diff = f1 - f2;
  diff = diff < 0 ? -diff : diff;
  return diff < epsilon;
}

static constexpr float  f_epsilon = 0.00001f;
static constexpr double d_epsilon = 0.00001;

TEST_CASE("cx_arg double test") {
  static constexpr auto d1 = 62.53_double;
  static constexpr auto d2 = -62.53_double;
  static constexpr auto d3 = +62.53_double;
  STATIC_REQUIRE(epsilon_equal(d1.value, d3.value, d_epsilon));
  STATIC_REQUIRE(epsilon_equal(d1.value, 62.53, d_epsilon));
  STATIC_REQUIRE(epsilon_equal(d2.value, -62.53, d_epsilon));
  STATIC_REQUIRE(d1.string == d3.string);
}
TEST_CASE("cx_arg float test") {
  static constexpr auto f1 = 62.53_float;
  static constexpr auto f2 = -62.53_float;
  static constexpr auto f3 = +62.53_float;
  STATIC_REQUIRE(epsilon_equal(f1.value, f3.value, f_epsilon));
  STATIC_REQUIRE(epsilon_equal(f1.value, 62.53f, f_epsilon));
  STATIC_REQUIRE(epsilon_equal(f2.value, -62.53f, f_epsilon));

  STATIC_REQUIRE(f1.string == f3.string);
}
