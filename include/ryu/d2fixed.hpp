// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
// Runtime compiler options:
// -DRYU_DEBUG Generate verbose debugging output to stdout.
//
// -DRYU_ONLY_64_BIT_OPS Avoid using uint128_t or 64-bit intrinsics. Slower,
//     depending on your compiler.
//
// -DRYU_AVOID_UINT128 Avoid using uint128_t. Slower, depending on your compiler.

#ifndef RYU_D2FIXED_HPP
#define RYU_D2FIXED_HPP

#include "ryu/d2s.hpp"

#include <cstdint>

namespace ryu::detail {
  // Convert `digits` to a sequence of decimal digits. Append the digits to the result.
  // The caller has to guarantee that:
  //   10^(olength-1) <= digits < 10^olength
  // e.g., by passing `olength` as `decimalLength9(digits)`.
  template <typename CharT>
  constexpr void
      append_n_digits(const uint32_t olength, uint32_t digits, CharT* const result) noexcept;

  // Convert `digits` to a sequence of decimal digits. Print the first digit, followed by a decimal
  // dot '.' followed by the remaining digits. The caller has to guarantee that:
  //   10^(olength-1) <= digits < 10^olength
  // e.g., by passing `olength` as `decimalLength9(digits)`.
  template <typename CharT>
  constexpr void
      append_d_digits(const uint32_t olength, uint32_t digits, CharT* const result) noexcept;

  // Convert `digits` to decimal and write the last `count` decimal digits to result.
  // If `digits` contains additional digits, then those are silently ignored.
  template <typename CharT>
  constexpr void
      append_c_digits(const uint32_t count, uint32_t digits, CharT* const result) noexcept;

  // Convert `digits` to decimal and write the last 9 decimal digits to result.
  // If `digits` contains additional digits, then those are silently ignored.
  template <typename CharT>
  constexpr void append_nine_digits(uint32_t digits, char* const result) noexcept;

  constexpr uint32_t indexForExponent(const uint32_t e) noexcept;

  constexpr uint32_t pow10BitsForIndex(const uint32_t idx) noexcept;

  constexpr uint32_t lengthForIndex(const uint32_t idx) noexcept;

  template <typename CharT>
  constexpr unsigned copy_special_str_printf(CharT* const   result,
                                             const bool     sign,
                                             const uint64_t mantissa) noexcept;
  template <typename CharT>
  constexpr unsigned d2fixed_buffered_n(double             d,
                                        uint32_t           precision,
                                        CharT*             result,
                                        DoubleCastFunction bit_cast) noexcept;

  template <typename CharT>
  constexpr unsigned d2exp_buffered_n(double             d,
                                      uint32_t           precision,
                                      CharT*             result,
                                      DoubleCastFunction bit_cast) noexcept;

} // namespace ryu::detail

#include "impl/d2fixed_impl.hpp"
#endif /* RYU_D2FIXED_HPP */
