// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_IMPL_D2S_IMPL_HPP
#define RYU_IMPL_D2S_IMPL_HPP
#include "ryu/common.hpp"
#include "ryu/d2s.hpp"
#include "ryu/d2s_intrinsics.hpp"
#include "ryu/digit_table.hpp"

// Include either the small or the full lookup tables depending on the mode.
#if defined(RYU_OPTIMIZE_SIZE)
  #include "ryu/d2s_small_table.hpp"
#else
  #include "ryu/d2s_full_table.hpp"
#endif

namespace ryu::detail {
  constexpr uint32_t decimalLength17(const uint64_t v) noexcept {
    // This is slightly faster than a loop.
    // The average output length is 16.38 digits, so we check high-to-low.
    // Function precondition: v is not an 18, 19, or 20-digit number.
    // (17 digits are sufficient for round-tripping.)
    // assert(v < 100000000000000000L);
    if (v >= 10000000000000000L) {
      return 17;
    }
    if (v >= 1000000000000000L) {
      return 16;
    }
    if (v >= 100000000000000L) {
      return 15;
    }
    if (v >= 10000000000000L) {
      return 14;
    }
    if (v >= 1000000000000L) {
      return 13;
    }
    if (v >= 100000000000L) {
      return 12;
    }
    if (v >= 10000000000L) {
      return 11;
    }
    if (v >= 1000000000L) {
      return 10;
    }
    if (v >= 100000000L) {
      return 9;
    }
    if (v >= 10000000L) {
      return 8;
    }
    if (v >= 1000000L) {
      return 7;
    }
    if (v >= 100000L) {
      return 6;
    }
    if (v >= 10000L) {
      return 5;
    }
    if (v >= 1000L) {
      return 4;
    }
    if (v >= 100L) {
      return 3;
    }
    if (v >= 10L) {
      return 2;
    }
    return 1;
  }
  constexpr floating_decimal_64 d2d(const uint64_t ieeeMantissa,
                                    const uint32_t ieeeExponent) noexcept {
    int32_t  e2{0};
    uint64_t m2{0};
    if (ieeeExponent == 0) {
      // We subtract 2 so that the bounds computation has 2 additional bits.
      e2 = 1 - double_bias - double_mantissa_bits - 2;
      m2 = ieeeMantissa;
    } else {
      e2 = (int32_t)ieeeExponent - double_bias - double_mantissa_bits - 2;
      m2 = (1ull << double_mantissa_bits) | ieeeMantissa;
    }
    const bool even = (m2 & 1) == 0;
    const bool acceptBounds = even;

    // Step 2: Determine the interval of valid decimal representations.
    const uint64_t mv = 4 * m2;
    // Implicit bool -> int conversion. True is 1, false is 0.
    const uint32_t mmShift = ieeeMantissa != 0 || ieeeExponent <= 1;
    // We would compute mp and mm like this:
    // uint64_t mp = 4 * m2 + 2;
    // uint64_t mm = mv - 1 - mmShift;

    // Step 3: Convert to a decimal power base using 128-bit arithmetic.
    uint64_t vr{0};
    uint64_t vp{0};
    uint64_t vm{0};
    int32_t  e10{0};
    bool     vmIsTrailingZeros = false;
    bool     vrIsTrailingZeros = false;
    if (e2 >= 0) {
      // I tried special-casing q == 0, but there was no effect on performance.
      // This expression is slightly faster than max(0, log10Pow2(e2) - 1).
      const uint32_t q = log10Pow2(e2) - (e2 > 3);
      e10 = (int32_t)q;
      const int32_t k = ryu::config::DOUBLE_POW5_INV_BITCOUNT + pow5bits((int32_t)q) - 1;
      const int32_t i = -e2 + (int32_t)q + k;
#if defined(RYU_OPTIMIZE_SIZE)
      uint64_t pow5[2];
      double_computeInvPow5(q, pow5);
      vr = mulShiftAll64(m2, pow5, i, &vp, &vm, mmShift);
#else
      vr = mulShiftAll64(m2, DOUBLE_POW5_INV_SPLIT[q], i, &vp, &vm, mmShift);
#endif

      if (q <= 21) {
        // This should use q <= 22, but I think 21 is also safe. Smaller values
        // may still be safe, but it's more difficult to reason about them.
        // Only one of mp, mv, and mm can be a multiple of 5, if any.
        const uint32_t mvMod5 = ((uint32_t)mv) - 5 * ((uint32_t)div5(mv));
        if (mvMod5 == 0) {
          vrIsTrailingZeros = multipleOfPowerOf5(mv, q);
        } else if (acceptBounds) {
          // Same as min(e2 + (~mm & 1), pow5Factor(mm)) >= q
          // <=> e2 + (~mm & 1) >= q && pow5Factor(mm) >= q
          // <=> true && pow5Factor(mm) >= q, since e2 >= q.
          vmIsTrailingZeros = multipleOfPowerOf5(mv - 1 - mmShift, q);
        } else {
          // Same as min(e2 + 1, pow5Factor(mp)) >= q.
          vp -= multipleOfPowerOf5(mv + 2, q);
        }
      }
    } else {
      // This expression is slightly faster than max(0, log10Pow5(-e2) - 1).
      const uint32_t q = log10Pow5(-e2) - (-e2 > 1);
      e10 = (int32_t)q + e2;
      const int32_t i = -e2 - (int32_t)q;
      const int32_t k = pow5bits(i) - ryu::config::DOUBLE_POW5_BITCOUNT;
      const int32_t j = (int32_t)q - k;
#if defined(RYU_OPTIMIZE_SIZE)
      uint64_t pow5[2];
      double_computePow5(i, pow5);
      vr = mulShiftAll64(m2, pow5, j, &vp, &vm, mmShift);
#else
      vr = mulShiftAll64(m2, DOUBLE_POW5_SPLIT[i], j, &vp, &vm, mmShift);
#endif

      if (q <= 1) {
        // {vr,vp,vm} is trailing zeros if {mv,mp,mm} has at least q trailing 0 bits.
        // mv = 4 * m2, so it always has at least two trailing 0 bits.
        vrIsTrailingZeros = true;
        if (acceptBounds) {
          // mm = mv - 1 - mmShift, so it has 1 trailing 0 bit iff mmShift == 1.
          vmIsTrailingZeros = mmShift == 1;
        } else {
          // mp = mv + 2, so it always has at least one trailing 0 bit.
          --vp;
        }
      } else if (q < 63) { // TODO(ulfjack): Use a tighter bound here.
        // We want to know if the full product has at least q trailing zeros.
        // We need to compute min(p2(mv), p5(mv) - e2) >= q
        // <=> p2(mv) >= q && p5(mv) - e2 >= q
        // <=> p2(mv) >= q (because -e2 >= q)
        vrIsTrailingZeros = multipleOfPowerOf2(mv, q);
      }
    }

    // Step 4: Find the shortest decimal representation in the interval of valid representations.
    int32_t  removed = 0;
    uint8_t  lastRemovedDigit = 0;
    uint64_t output{0};
    // On average, we remove ~2 digits.
    if (vmIsTrailingZeros || vrIsTrailingZeros) {
      // General case, which happens rarely (~0.7%).
      for (;;) {
        const uint64_t vpDiv10 = div10(vp);
        const uint64_t vmDiv10 = div10(vm);
        if (vpDiv10 <= vmDiv10) {
          break;
        }
        const uint32_t vmMod10 = ((uint32_t)vm) - 10 * ((uint32_t)vmDiv10);
        const uint64_t vrDiv10 = div10(vr);
        const uint32_t vrMod10 = ((uint32_t)vr) - 10 * ((uint32_t)vrDiv10);
        vmIsTrailingZeros &= vmMod10 == 0;
        vrIsTrailingZeros &= lastRemovedDigit == 0;
        lastRemovedDigit = (uint8_t)vrMod10;
        vr = vrDiv10;
        vp = vpDiv10;
        vm = vmDiv10;
        ++removed;
      }

      if (vmIsTrailingZeros) {
        for (;;) {
          const uint64_t vmDiv10 = div10(vm);
          const uint32_t vmMod10 = ((uint32_t)vm) - 10 * ((uint32_t)vmDiv10);
          if (vmMod10 != 0) {
            break;
          }
          const uint64_t vpDiv10 = div10(vp);
          const uint64_t vrDiv10 = div10(vr);
          const uint32_t vrMod10 = ((uint32_t)vr) - 10 * ((uint32_t)vrDiv10);
          vrIsTrailingZeros &= lastRemovedDigit == 0;
          lastRemovedDigit = (uint8_t)vrMod10;
          vr = vrDiv10;
          vp = vpDiv10;
          vm = vmDiv10;
          ++removed;
        }
      }

      if (vrIsTrailingZeros && lastRemovedDigit == 5 && vr % 2 == 0) {
        // Round even if the exact number is .....50..0.
        lastRemovedDigit = 4;
      }
      // We need to take vr + 1 if vr is outside bounds or we need to round up.
      output = vr + ((vr == vm && (!acceptBounds || !vmIsTrailingZeros)) || lastRemovedDigit >= 5);
    } else {
      // Specialized for the common case (~99.3%). Percentages below are relative to this.
      bool           roundUp = false;
      const uint64_t vpDiv100 = div100(vp);
      const uint64_t vmDiv100 = div100(vm);
      if (vpDiv100 > vmDiv100) { // Optimization: remove two digits at a time (~86.2%).
        const uint64_t vrDiv100 = div100(vr);
        const uint32_t vrMod100 = ((uint32_t)vr) - 100 * ((uint32_t)vrDiv100);
        roundUp = vrMod100 >= 50;
        vr = vrDiv100;
        vp = vpDiv100;
        vm = vmDiv100;
        removed += 2;
      }
      // Loop iterations below (approximately), without optimization above:
      // 0: 0.03%, 1: 13.8%, 2: 70.6%, 3: 14.0%, 4: 1.40%, 5: 0.14%, 6+: 0.02%
      // Loop iterations below (approximately), with optimization above:
      // 0: 70.6%, 1: 27.8%, 2: 1.40%, 3: 0.14%, 4+: 0.02%
      for (;;) {
        const uint64_t vpDiv10 = div10(vp);
        const uint64_t vmDiv10 = div10(vm);
        if (vpDiv10 <= vmDiv10) {
          break;
        }
        const uint64_t vrDiv10 = div10(vr);
        const uint32_t vrMod10 = ((uint32_t)vr) - 10 * ((uint32_t)vrDiv10);
        roundUp = vrMod10 >= 5;
        vr = vrDiv10;
        vp = vpDiv10;
        vm = vmDiv10;
        ++removed;
      }

      // We need to take vr + 1 if vr is outside bounds or we need to round up.
      output = vr + (vr == vm || roundUp);
    }
    const int32_t exp = e10 + removed;

    floating_decimal_64 fd{};
    fd.exponent = exp;
    fd.mantissa = output;
    return fd;
  }
  template <typename CharT>
  constexpr unsigned
      to_chars(const floating_decimal_64 v, const bool sign, CharT* const result) noexcept {
    // Step 5: Print the decimal representation.
    unsigned index = 0;
    if (sign) {
      result[index++] = static_cast<CharT>('-');
    }

    uint64_t       output = v.mantissa;
    const uint32_t olength = decimalLength17(output);

    // Print the decimal digits.
    // The following code is equivalent to:
    // for (uint32_t i = 0; i < olength - 1; ++i) {
    //   const uint32_t c = output % 10; output /= 10;
    //   result[index + olength - i] = (char) ('0' + c);
    // }
    // result[index] = '0' + output % 10;

    uint32_t i = 0;
    // We prefer 32-bit operations, even on 64-bit platforms.
    // We have at most 17 digits, and uint32_t can store 9 digits.
    // If output doesn't fit into uint32_t, we cut off 8 digits,
    // so the rest will fit into uint32_t.
    if ((output >> 32) != 0) {
      // Expensive 64-bit division.
      const uint64_t q = div1e8(output);
      uint32_t       output2 = ((uint32_t)output) - 100000000 * ((uint32_t)q);
      output = q;

      const uint32_t c = output2 % 10000;
      output2 /= 10000;
      const uint32_t d = output2 % 10000;
      const uint32_t c0 = (c % 100) << 1;
      const uint32_t c1 = (c / 100) << 1;
      const uint32_t d0 = (d % 100) << 1;
      const uint32_t d1 = (d / 100) << 1;
      // memcpy(result + index + olength - i - 1, DIGIT_TABLE + c0, 2);
      result[index + olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c0]);
      result[index + olength - i - 0] = static_cast<CharT>(DIGIT_TABLE[c0 + 1]);

      // memcpy(result + index + olength - i - 3, DIGIT_TABLE + c1, 2);
      result[index + olength - i - 3] = static_cast<CharT>(DIGIT_TABLE[c1]);
      result[index + olength - i - 2] = static_cast<CharT>(DIGIT_TABLE[c1 + 1]);

      // memcpy(result + index + olength - i - 5, DIGIT_TABLE + d0, 2);
      result[index + olength - i - 5] = static_cast<CharT>(DIGIT_TABLE[d0]);
      result[index + olength - i - 4] = static_cast<CharT>(DIGIT_TABLE[d0 + 1]);

      // memcpy(result + index + olength - i - 7, DIGIT_TABLE + d1, 2);
      result[index + olength - i - 7] = static_cast<CharT>(DIGIT_TABLE[d1]);
      result[index + olength - i - 6] = static_cast<CharT>(DIGIT_TABLE[d1 + 1]);
      i += 8;
    }
    uint32_t output2 = static_cast<uint32_t>(output);
    while (output2 >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
      const uint32_t c = output2 - 10000 * (output2 / 10000);
#else
      const uint32_t c = output2 % 10000;
#endif
      output2 /= 10000;
      const uint32_t c0 = (c % 100) << 1;
      const uint32_t c1 = (c / 100) << 1;
      // memcpy(result + index + olength - i - 1, DIGIT_TABLE + c0, 2);
      result[index + olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c0]);
      result[index + olength - i - 0] = static_cast<CharT>(DIGIT_TABLE[c0 + 1]);
      // memcpy(result + index + olength - i - 3, DIGIT_TABLE + c1, 2);
      result[index + olength - i - 3] = static_cast<CharT>(DIGIT_TABLE[c1]);
      result[index + olength - i - 2] = static_cast<CharT>(DIGIT_TABLE[c1 + 1]);
      i += 4;
    }
    if (output2 >= 100) {
      const uint32_t c = (output2 % 100) << 1;
      output2 /= 100;
      // memcpy(result + index + olength - i - 1, DIGIT_TABLE + c, 2);
      result[index + olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c]);
      result[index + olength - i - 0] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
      i += 2;
    }
    if (output2 >= 10) {
      const uint32_t c = output2 << 1;
      // We can't use memcpy here: the decimal dot goes between these two digits.
      result[index + olength - i] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
      result[index] = static_cast<CharT>(DIGIT_TABLE[c]);
    } else {
      result[index] = static_cast<CharT>('0' + output2);
    }

    // Print decimal point if needed.
    if (olength > 1) {
      result[index + 1] = static_cast<CharT>('.');
      index += olength + 1;
    } else {
      ++index;
    }

    // Print the exponent.
    result[index++] = static_cast<CharT>('E');
    int32_t exp = v.exponent + (int32_t)olength - 1;
    if (exp < 0) {
      result[index++] = static_cast<CharT>('-');
      exp = -exp;
    }

    if (exp >= 100) {
      const int32_t c = exp % 10;
      const auto    digit_idx = 2 * (exp / 10);
      // memcpy(result + index, DIGIT_TABLE + 2 * (exp / 10), 2);
      result[index] = static_cast<CharT>(DIGIT_TABLE[digit_idx]);
      result[index + 1] = static_cast<CharT>(DIGIT_TABLE[digit_idx + 1]);
      result[index + 2] = static_cast<CharT>('0' + c);
      index += 3;
    } else if (exp >= 10) {
      const auto digit_idx = 2 * exp;
      // memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
      result[index] = static_cast<CharT>(DIGIT_TABLE[digit_idx]);
      result[index + 1] = static_cast<CharT>(DIGIT_TABLE[digit_idx + 1]);
      index += 2;
    } else {
      result[index++] = static_cast<CharT>('0' + exp);
    }

    return index;
  }

  constexpr bool d2d_small_int(const uint64_t             ieeeMantissa,
                               const uint32_t             ieeeExponent,
                               floating_decimal_64* const v) noexcept {
    const uint64_t m2 = (1ull << double_mantissa_bits) | ieeeMantissa;
    const int32_t  e2 = (int32_t)ieeeExponent - double_bias - double_mantissa_bits;

    if (e2 > 0) {
      // f = m2 * 2^e2 >= 2^53 is an integer.
      // Ignore this case for now.
      return false;
    }

    if (e2 < -52) {
      // f < 1.
      return false;
    }

    // Since 2^52 <= m2 < 2^53 and 0 <= -e2 <= 52: 1 <= f = m2 / 2^-e2 < 2^53.
    // Test if the lower -e2 bits of the significand are 0, i.e. whether the fraction is 0.
    const uint64_t mask = (1ull << -e2) - 1;
    const uint64_t fraction = m2 & mask;
    if (fraction != 0) {
      return false;
    }

    // f is an integer in the range [1, 2^53).
    // Note: mantissa might contain trailing (decimal) 0's.
    // Note: since 2^53 < 10^16, there is no need to adjust decimalLength17().
    v->mantissa = m2 >> -e2;
    v->exponent = 0;
    return true;
  }

  template <typename CharT>
  constexpr unsigned d2s_buffered_n(double f, CharT* result, DoubleCastFunction bit_cast) noexcept {
    // Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
    const uint64_t bits = bit_cast(f);

    // Decode bits into sign, mantissa, and exponent.
    const bool     ieeeSign = ((bits >> (double_mantissa_bits + double_exponent_bits)) & 1) != 0;
    const uint64_t ieeeMantissa = bits & ((1ull << double_mantissa_bits) - 1);
    const uint32_t ieeeExponent =
        static_cast<uint32_t>((bits >> double_mantissa_bits) & ((1u << double_exponent_bits) - 1));
    // Case distinction; exit early for the easy cases.
    if (ieeeExponent == ((1u << double_exponent_bits) - 1u) ||
        (ieeeExponent == 0 && ieeeMantissa == 0)) {
      return copy_special_str(result, ieeeSign, ieeeExponent, ieeeMantissa);
    }

    floating_decimal_64 v;
    const bool          isSmallInt = d2d_small_int(ieeeMantissa, ieeeExponent, &v);
    if (isSmallInt) {
      // For small integers in the range [1, 2^53), v.mantissa might contain trailing (decimal)
      // zeros. For scientific notation we need to move these zeros into the exponent. (This is not
      // needed for fixed-point notation, so it might be beneficial to trim trailing zeros in
      // to_chars only if needed - once fixed-point notation output is implemented.)
      for (;;) {
        const uint64_t q = div10(v.mantissa);
        const uint32_t r = ((uint32_t)v.mantissa) - 10 * ((uint32_t)q);
        if (r != 0) {
          break;
        }
        v.mantissa = q;
        ++v.exponent;
      }
    } else {
      v = d2d(ieeeMantissa, ieeeExponent);
    }

    return to_chars(v, ieeeSign, result);
  }

} // namespace ryu::detail

namespace ryu {
  template <typename CharT>
  inline unsigned d2s_buffered_n(double f, CharT* result) noexcept {
    return ryu::detail::d2s_buffered_n(f, result, &ryu::to_bits);
  }

  namespace cx {
    template <typename CharT>
    constexpr unsigned d2s_buffered_n(double f, CharT* result) noexcept {
      return ryu::detail::d2s_buffered_n(f, result, &ryu::cx::to_bits);
    }
  } // namespace cx
} // namespace ryu

#endif /* RYU_IMPL_D2S_IMPL_HPP */
