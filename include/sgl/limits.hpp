
/// \file limits.hpp
/// \author Pelé Constam (you\domain.com)
/// \brief This file defines a class template called numeric_limits, analogous to
/// std::numeric_limits.
/// \version 0.1
/// \date 2022-06-14
///
/// \copyright Copyright (c) 2022
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_LIMITS_HPP
#define SGL_LIMITS_HPP
/// \cond

#include <cfloat>
#include <climits>
#include <cstdint>
#include <cwchar>

namespace sgl {
  template <typename T>
  struct numeric_limits {
    static constexpr T max() noexcept;
    static constexpr T min() noexcept;
  };
  template <>
  struct numeric_limits<bool> {
    static constexpr bool max() noexcept { return true; }
    static constexpr bool min() noexcept { return false; }
  };
  template <>
  struct numeric_limits<char> {
    static constexpr char max() noexcept { return CHAR_MAX; }
    static constexpr char min() noexcept { return CHAR_MIN; }
  };
  template <>
  struct numeric_limits<signed char> {
    static constexpr signed char max() noexcept { return SCHAR_MAX; }
    static constexpr signed char min() noexcept { return SCHAR_MIN; }
  };
  template <>
  struct numeric_limits<unsigned char> {
    static constexpr unsigned char max() noexcept { return UCHAR_MAX; }
    static constexpr unsigned char min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<wchar_t> {
    static constexpr wchar_t max() noexcept { return WCHAR_MAX; }
    static constexpr wchar_t min() noexcept { return WCHAR_MIN; }
  };
  template <>
  struct numeric_limits<char16_t> {
    static constexpr char16_t max() noexcept { return UINT_LEAST16_MAX; }
    static constexpr char16_t min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<char32_t> {
    static constexpr char32_t max() noexcept { return UINT_LEAST32_MAX; }
    static constexpr char32_t min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<short> {
    static constexpr short max() noexcept { return SHRT_MAX; }
    static constexpr short min() noexcept { return SHRT_MIN; }
  };
  template <>
  struct numeric_limits<unsigned short> {
    static constexpr unsigned short max() noexcept { return USHRT_MAX; }
    static constexpr unsigned short min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<int> {
    static constexpr int max() noexcept { return INT_MAX; }
    static constexpr int min() noexcept { return INT_MIN; }
  };
  template <>
  struct numeric_limits<unsigned int> {
    static constexpr unsigned int max() noexcept { return UINT_MAX; }
    static constexpr unsigned int min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<long> {
    static constexpr long max() noexcept { return LONG_MAX; }
    static constexpr long min() noexcept { return LONG_MIN; }
  };
  template <>
  struct numeric_limits<unsigned long> {
    static constexpr unsigned long max() noexcept { return ULONG_MAX; }
    static constexpr unsigned long min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<long long> {
    static constexpr long long max() noexcept { return LLONG_MAX; }
    static constexpr long long min() noexcept { return LLONG_MIN; }
  };
  template <>
  struct numeric_limits<unsigned long long> {
    static constexpr unsigned long long max() noexcept { return ULLONG_MAX; }
    static constexpr unsigned long long min() noexcept { return 0; }
  };
  template <>
  struct numeric_limits<float> {
    static constexpr float max() noexcept { return FLT_MAX; }
    static constexpr float min() noexcept { return FLT_MIN; }
  };
  template <>
  struct numeric_limits<double> {
    static constexpr double max() noexcept { return LDBL_MAX; }
    static constexpr double min() noexcept { return DBL_MIN; }
  };
  template <>
  struct numeric_limits<long double> {
    static constexpr long double max() noexcept { return LDBL_MAX; }
    static constexpr long double min() noexcept { return LDBL_MIN; }
  };
  /// \endcond
} // namespace sgl

#endif /* SGL_LIMITS_HPP */
