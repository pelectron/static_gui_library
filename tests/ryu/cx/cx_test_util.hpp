#ifndef RYU_CX_UTIL_HPP
#define RYU_CX_UTIL_HPP
#include "gcem.hpp"
#include "ryu/ryu.hpp"
constexpr bool cmp(const char* s1, const char* s2, size_t size) {
  for (size_t i = 0; i < size; ++i) {
    if (s1[i] != s2[i])
      return false;
  }
  return true;
}

constexpr double
    ieeeParts2Double(const bool sign, const int ieeeExponent, const uint64_t ieeeMantissa) {
  return (sign ? -1 : 1) * (1.0 + ((double)ieeeMantissa) / gcem::pow(2.0, 53.0)) *
         gcem::pow(2.0, ieeeExponent - ryu::double_bias);
}


#endif /* RYU_CX_UTIL_HPP */
