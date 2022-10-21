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
// -DRYU_OPTIMIZE_SIZE Use smaller lookup tables. Instead of storing every
//     required power of 5, only store every 26th entry, and compute
//     intermediate values with a multiplication. This reduces the lookup table
//     size by about 10x (only one case, and only double) at the cost of some
//     performance. Currently requires MSVC intrinsics.
#ifndef RYU_D2S_HPP
#define RYU_D2S_HPP

#include <cstdint>

namespace ryu::detail {
  using DoubleCastFunction = uint64_t (*)(const double) noexcept;

  // A floating decimal representing m * 10^e.
  struct floating_decimal_64 {
    uint64_t mantissa{0};
    // Decimal exponent's range is -324 to 308
    // inclusive, and can fit in a short if needed.
    int32_t exponent{0};
  };

  constexpr uint32_t decimalLength17(const uint64_t v) noexcept;

  constexpr floating_decimal_64 d2d(const uint64_t ieeeMantissa,
                                    const uint32_t ieeeExponent) noexcept;
  template <typename CharT>
  constexpr unsigned
      to_chars(const floating_decimal_64 v, const bool sign, CharT* const result) noexcept;

  constexpr bool d2d_small_int(const uint64_t             ieeeMantissa,
                               const uint32_t             ieeeExponent,
                               floating_decimal_64* const v) noexcept;

  template <typename CharT>
  constexpr unsigned d2s_buffered_n(double f, CharT* result, DoubleCastFunction bit_cast) noexcept;

} // namespace ryu::detail

#include "impl/d2s_impl.hpp"
#endif /* RYU_D2S_HPP */
