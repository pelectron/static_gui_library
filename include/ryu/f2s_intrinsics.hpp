// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_F2S_INTRINSICS_HPP
#define RYU_F2S_INTRINSICS_HPP
#include "ryu/common.hpp"

#include <cstdint>

#if defined(RYU_FLOAT_FULL_TABLE)

  #include "ryu/f2s_full_table.hpp"

#else

  #if defined(RYU_OPTIMIZE_SIZE)
    #include "ryu/d2s_small_table.hpp"
  #else
    #include "ryu/d2s_full_table.hpp"
  #endif
namespace ryu::detail {
  inline constexpr int float_pow5_inv_bitcount = config::DOUBLE_POW5_INV_BITCOUNT - 64;
  inline constexpr int float_pow5_bitcount = config::DOUBLE_POW5_BITCOUNT - 64;
} // namespace ryu::detail

#endif

namespace ryu::detail {
  constexpr uint32_t pow5factor_32(uint32_t value) {
    uint32_t count = 0;
    for (;;) {
      // // assert(value != 0);
      const uint32_t q = value / 5;
      const uint32_t r = value % 5;
      if (r != 0) {
        break;
      }
      value = q;
      ++count;
    }
    return count;
  }

  // Returns true if value is divisible by 5^p.
  constexpr bool multipleOfPowerOf5_32(const uint32_t value, const uint32_t p) {
    return pow5factor_32(value) >= p;
  }

  // Returns true if value is divisible by 2^p.
  constexpr bool multipleOfPowerOf2_32(const uint32_t value, const uint32_t p) {
    // __builtin_ctz doesn't appear to be faster here.
    return (value & ((1u << p) - 1)) == 0;
  }

  // It seems to be slightly faster to avoid uint128_t here, although the
  // generated code for uint128_t looks slightly nicer.
  constexpr uint32_t mulShift32(const uint32_t m, const uint64_t factor, const int32_t shift) {
    // assert(shift > 32);

    // The casts here help MSVC to avoid calls to the __allmul library
    // function.
    const uint32_t factorLo = (uint32_t)(factor);
    const uint32_t factorHi = (uint32_t)(factor >> 32);
    const uint64_t bits0 = (uint64_t)m * factorLo;
    const uint64_t bits1 = (uint64_t)m * factorHi;

#if defined(RYU_32_BIT_PLATFORM)
    // On 32-bit platforms we can avoid a 64-bit shift-right since we only
    // need the upper 32 bits of the result and the shift value is > 32.
    const uint32_t bits0Hi = (uint32_t)(bits0 >> 32);
    uint32_t       bits1Lo = (uint32_t)(bits1);
    uint32_t       bits1Hi = (uint32_t)(bits1 >> 32);
    bits1Lo += bits0Hi;
    bits1Hi += (bits1Lo < bits0Hi);
    if (shift >= 64) {
      // s2f can call this with a shift value >= 64, which we have to handle.
      // This could now be slower than the !defined(RYU_32_BIT_PLATFORM) case.
      return (uint32_t)(bits1Hi >> (shift - 64));
    } else {
      const int32_t s = shift - 32;
      return (bits1Hi << (32 - s)) | (bits1Lo >> s);
    }
#else  // RYU_32_BIT_PLATFORM
    const uint64_t sum = (bits0 >> 32) + bits1;
    const uint64_t shiftedSum = sum >> (shift - 32);
    // assert(shiftedSum <= UINT32_MAX);
    return (uint32_t)shiftedSum;
#endif // RYU_32_BIT_PLATFORM
  }

  constexpr uint32_t mulPow5InvDivPow2(const uint32_t m, const uint32_t q, const int32_t j) {
#if defined(RYU_FLOAT_FULL_TABLE)
    return mulShift32(m, detail::FLOAT_POW5_INV_SPLIT[q], j);
#elif defined(RYU_OPTIMIZE_SIZE)
    // The inverse multipliers are defined as [2^x / 5^y] + 1; the upper 64 bits from the double
    // lookup table are the correct bits for [2^x / 5^y], so we have to add 1 here. Note that we
    // rely on the fact that the added 1 that's already stored in the table never overflows into the
    // upper 64 bits.
    uint64_t pow5[2];
    double_computeInvPow5(q, pow5);
    return mulShift32(m, pow5[1] + 1, j);
#else
    return mulShift32(m, detail::DOUBLE_POW5_INV_SPLIT[q][1] + 1, j);
#endif
  }

  constexpr uint32_t mulPow5divPow2(const uint32_t m, const uint32_t i, const int32_t j) {
#if defined(RYU_FLOAT_FULL_TABLE)
    return mulShift32(m, detail::FLOAT_POW5_SPLIT[i], j);
#elif defined(RYU_OPTIMIZE_SIZE)
    uint64_t pow5[2];
    double_computePow5(i, pow5);
    return mulShift32(m, pow5[1], j);
#else
    return mulShift32(m, detail::DOUBLE_POW5_SPLIT[i][1], j);
#endif
  }

} // namespace ryu::detail

#endif /* RYU_F2S_INTRINSICS_HPP */
