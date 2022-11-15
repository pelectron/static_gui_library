//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_RYU_TEST_UTIL_HPP
#define RYU_RYU_TEST_UTIL_HPP
#include <iomanip>
#include <sstream>

inline std::string FullPrecision(double d) {
  auto s = std::ostringstream{};
  s << std::setprecision(std::numeric_limits<double>::max_digits10) << d;
  return s.str();
}

#endif /* RYU_RYU_TEST_UTIL_HPP */
