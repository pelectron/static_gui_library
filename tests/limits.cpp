#include "sgl/limits.hpp"

#include <catch2/catch.hpp>
#include <limits>


TEMPLATE_TEST_CASE("limits",
                   "[template]",
                   bool,
                   char,
                   (signed char),
                   (unsigned char),
                   wchar_t,
                   char16_t,
                   char32_t,
                   short,
                   (unsigned short),
                   int,
                   (unsigned int),
                   long,
                   (unsigned long),
                   (long long),
                   (unsigned long long),
                   float,
                   double,
                   long double) {
  REQUIRE(sgl::numeric_limits<TestType>::min() == std::numeric_limits<TestType>::min());
  REQUIRE(sgl::numeric_limits<TestType>::max() == std::numeric_limits<TestType>::max());
}