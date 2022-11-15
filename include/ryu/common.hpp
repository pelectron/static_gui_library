// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_COMMON_HPP
#define RYU_COMMON_HPP
#include <cmath>
#include <cstdint>
#include <cstring>

#if defined(_M_IX86) || defined(_M_ARM)
  #define RYU_32_BIT_PLATFORM
#endif
namespace ryu {

  enum class status { success, input_too_short, input_too_long, malformed_input };

  inline constexpr int    float_mantissa_bits = 23;
  inline constexpr int    float_exponent_bits = 8;
  inline constexpr int    float_bias = 127;
  inline constexpr int    double_mantissa_bits = 52;
  inline constexpr int    double_exponent_bits = 11;
  inline constexpr int    double_bias = 1023;
  inline constexpr int    POW10_ADDITIONAL_BITS = 120;
  inline constexpr float  f_infinity = HUGE_VALL;
  inline constexpr double d_infinity = HUGE_VALL;

  namespace config {
    inline constexpr int DOUBLE_POW5_BITCOUNT = 125;
    inline constexpr int DOUBLE_POW5_INV_BITCOUNT = 125;
  } // namespace config

  namespace detail {
    /// Returns the number of decimal digits in v, which must not contain more than 9 digits.
    constexpr uint32_t decimalLength9(const uint32_t v) noexcept;

    /// Returns e == 0 ? 1 : [log_2(5^e)]; requires 0 <= e <= 3528.
    constexpr int32_t log2pow5(const int32_t e) noexcept;

    /// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
    constexpr int32_t pow5bits(const int32_t e) noexcept;

    /// Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
    constexpr int32_t ceil_log2pow5(const int32_t e) noexcept;

    /// Returns floor(log_10(2^e)); requires 0 <= e <= 1650.
    constexpr uint32_t log10Pow2(const int32_t e) noexcept;

    /// Returns floor(log_10(5^e)); requires 0 <= e <= 2620.
    constexpr uint32_t log10Pow5(const int32_t e) noexcept;

    template <typename CharT>
    constexpr unsigned copy_special_str(CharT* const result,
                                        const bool   sign,
                                        const bool   exponent,
                                        const bool   mantissa) noexcept;

    static inline uint32_t floor_log2(const uint32_t value) noexcept;
    static inline uint32_t floor_log2(const uint64_t value) noexcept;
    constexpr int32_t      max32(int32_t a, int32_t b) noexcept;
  } // namespace detail

  namespace cx {
    constexpr uint32_t floor_log2(const uint32_t value) noexcept;
  }

  /// @brief get the bits of f as a uint32_t efficiently at runtime. Uses memcpy.
  /// @param f float to convert
  /// @return bit representation of f
  inline uint64_t to_bits(const double f) noexcept;

  /// @brief get the bits of f as a uint64_t efficiently at runtime. Uses memcpy.
  /// @param f double to convert
  /// @return bit representation of f
  inline uint32_t to_bits(const float f) noexcept;

  namespace cx {
    /// @brief get the bits of f in a constexpr friendly way. Use this function if and only if
    /// compile time formatting.
    /// @note Do not use at runtime, as the code is very inefficient
    /// compared to a memcpy! Use ryu::to_bits() at runtime.
    /// @param f float to convert
    /// @return bit representation of f
    constexpr uint32_t to_bits(const float f) noexcept;

    /// @brief get the bits of f in a constexpr friendly way. Use this function if and only if
    /// compile time formatting.
    /// @note Do not use at runtime, as the code is very inefficient
    /// compared to a memcpy! Use ryu::to_bits() at runtime.
    /// @param f float to convert
    /// @return bit representation of f
    constexpr uint64_t to_bits(const double f) noexcept;
  } // namespace cx

} // namespace ryu

#include "impl/common_impl.hpp"
#endif /* RYU_COMMON_HPP */
