// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_IMPL_D2FIXED_IMPL_HPP
#define RYU_IMPL_D2FIXED_IMPL_HPP
#include "ryu/common.hpp"
#include "ryu/d2fixed.hpp"
#include "ryu/d2fixed_full_table.hpp"
#include "ryu/d2s_intrinsics.hpp"
#include "ryu/digit_table.hpp"

namespace ryu::detail {

#if defined(HAS_UINT128)
  constexpr uint128_t umul256(const uint128_t  a,
                              const uint64_t   bHi,
                              const uint64_t   bLo,
                              uint128_t* const productHi) {
    const uint64_t aLo = static_cast<uint64_t>(a);
    const uint64_t aHi = static_cast<uint64_t>(a >> 64);

    const uint128_t b00 = (uint128_t)aLo * bLo;
    const uint128_t b01 = (uint128_t)aLo * bHi;
    const uint128_t b10 = (uint128_t)aHi * bLo;
    const uint128_t b11 = (uint128_t)aHi * bHi;

    const uint64_t b00Lo = static_cast<uint64_t>(b00);
    const uint64_t b00Hi = static_cast<uint64_t>(b00 >> 64);

    const uint128_t mid1 = b10 + b00Hi;
    const uint64_t  mid1Lo = static_cast<uint64_t>(mid1);
    const uint64_t  mid1Hi = static_cast<uint64_t>(mid1 >> 64);

    const uint128_t mid2 = b01 + mid1Lo;
    const uint64_t  mid2Lo = static_cast<uint64_t>(mid2);
    const uint64_t  mid2Hi = static_cast<uint64_t>(mid2 >> 64);

    const uint128_t pHi = b11 + mid1Hi + mid2Hi;
    const uint128_t pLo = (static_cast<uint128_t>(mid2Lo) << 64) | b00Lo;

    *productHi = pHi;
    return pLo;
  }

  // Returns the high 128 bits of the 256-bit product of a and b.
  constexpr uint128_t umul256_hi(const uint128_t a, const uint64_t bHi, const uint64_t bLo) {
    // Reuse the umul256 implementation.
    // Optimizers will likely eliminate the instructions used to compute the
    // low part of the product.
    uint128_t hi;
    umul256(a, bHi, bLo, &hi);
    return hi;
  }

  // Unfortunately, gcc/clang do not automatically turn a 128-bit integer division
  // into a multiplication, so we have to do it manually.
  constexpr uint32_t uint128_mod1e9(const uint128_t v) {
    // After multiplying, we're going to shift right by 29, then truncate to uint32_t.
    // This means that we need only 29 + 32 = 61 bits, so we can truncate to uint64_t before
    // shifting.
    const uint64_t multiplied =
        static_cast<uint64_t>(umul256_hi(v, 0x89705F4136B4A597u, 0x31680A88F8953031u));

    // For uint32_t truncation, see the mod1e9() comment in d2s_intrinsics.h.
    const uint32_t shifted = static_cast<uint32_t>(multiplied >> 29);

    return static_cast<uint32_t>(v) - 1000000000 * shifted;
  }

  // Best case: use 128-bit type.
  constexpr uint32_t mulShift_mod1e9(const uint64_t m, const uint64_t* const mul, const int32_t j) {
    const uint128_t b0 = ((uint128_t)m) * mul[0]; // 0
    const uint128_t b1 = ((uint128_t)m) * mul[1]; // 64
    const uint128_t b2 = ((uint128_t)m) * mul[2]; // 128

    // assert(j >= 128);
    // assert(j <= 180);
    // j: [128, 256)
    const uint128_t mid = b1 + (uint64_t)(b0 >> 64); // 64
    const uint128_t s1 = b2 + (uint64_t)(mid >> 64); // 128
    return uint128_mod1e9(s1 >> (j - 128));
  }

#else // HAS_UINT128

  #if defined(HAS_64_BIT_INTRINSICS)
  // Returns the low 64 bits of the high 128 bits of the 256-bit product of a and b.
  constexpr uint64_t umul256_hi128_lo64(const uint64_t aHi,
                                        const uint64_t aLo,
                                        const uint64_t bHi,
                                        const uint64_t bLo) {
    uint64_t       b00Hi{0};
    const uint64_t b00Lo = umul128(aLo, bLo, &b00Hi);
    uint64_t       b01Hi{0};
    const uint64_t b01Lo = umul128(aLo, bHi, &b01Hi);
    uint64_t       b10Hi{0};
    const uint64_t b10Lo = umul128(aHi, bLo, &b10Hi);
    uint64_t       b11Hi{0};
    const uint64_t b11Lo = umul128(aHi, bHi, &b11Hi);
    (void)b00Lo; // unused
    (void)b11Hi; // unused
    const uint64_t temp1Lo = b10Lo + b00Hi;
    const uint64_t temp1Hi = b10Hi + (temp1Lo < b10Lo);
    const uint64_t temp2Lo = b01Lo + temp1Lo;
    const uint64_t temp2Hi = b01Hi + (temp2Lo < b01Lo);
    return b11Lo + temp1Hi + temp2Hi;
  }

  constexpr uint32_t uint128_mod1e9(const uint64_t vHi, const uint64_t vLo) {
    // After multiplying, we're going to shift right by 29, then truncate to uint32_t.
    // This means that we need only 29 + 32 = 61 bits, so we can truncate to uint64_t before
    // shifting.
    const uint64_t multiplied =
        umul256_hi128_lo64(vHi, vLo, 0x89705F4136B4A597u, 0x31680A88F8953031u);

    // For uint32_t truncation, see the mod1e9() comment in d2s_intrinsics.h.
    const uint32_t shifted = static_cast<uint32_t>(multiplied >> 29);

    return static_cast<uint32_t>(vLo) - 1000000000 * shifted;
  }
  #endif // HAS_64_BIT_INTRINSICS

  constexpr uint32_t mulShift_mod1e9(const uint64_t m, const uint64_t* const mul, const int32_t j) {
    uint64_t       high0{0};                          // 64
    const uint64_t low0 = umul128(m, mul[0], &high0); // 0
    uint64_t       high1{0};                          // 128
    const uint64_t low1 = umul128(m, mul[1], &high1); // 64
    uint64_t       high2{0};                          // 192
    const uint64_t low2 = umul128(m, mul[2], &high2); // 128
    const uint64_t s0low = low0;                      // 0
    (void)s0low;                                      // unused
    const uint64_t s0high = low1 + high0;             // 64
    const uint32_t c1 = s0high < low1;
    const uint64_t s1low = low2 + high1 + c1;  // 128
    const uint32_t c2 = s1low < low2;          // high1 + c1 can't overflow, so compare against low2
    const uint64_t s1high = high2 + c2;        // 192
                                               // assert(j >= 128);
                                               // assert(j <= 180);
  #if defined(HAS_64_BIT_INTRINSICS)
    const uint32_t dist = (uint32_t)(j - 128); // dist: [0, 52]
    const uint64_t shiftedhigh = s1high >> dist;
    const uint64_t shiftedlow = shiftright128(s1low, s1high, dist > 63 ? 63 : dist);
    return uint128_mod1e9(shiftedhigh, shiftedlow);
  #else // HAS_64_BIT_INTRINSICS
    if (j < 160) { // j: [128, 160)
      const uint64_t r0 = mod1e9(s1high);
      const uint64_t r1 = mod1e9((r0 << 32) | (s1low >> 32));
      const uint64_t r2 = ((r1 << 32) | (s1low & 0xffffffff));
      return mod1e9(r2 >> (j - 128));
    }
    // j: [160, 192)
    const uint64_t r0 = mod1e9(s1high);
    const uint64_t r1 = ((r0 << 32) | (s1low >> 32));
    return mod1e9(r1 >> (j - 160));

  #endif // HAS_64_BIT_INTRINSICS
  }
#endif   // HAS_UINT128
  template <typename CharT>
  constexpr void
      append_n_digits(const uint32_t olength, uint32_t digits, CharT* const result) noexcept {

    uint32_t i = 0;
    while (digits >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
      const uint32_t c = digits - 10000 * (digits / 10000);
#else
      const uint32_t c = digits % 10000;
#endif
      digits /= 10000;
      const uint32_t c0 = (c % 100) << 1;
      const uint32_t c1 = (c / 100) << 1;
      // memcpy(result + olength - i - 2, DIGIT_TABLE + c0, 2);
      result[olength - i - 2] = static_cast<CharT>(DIGIT_TABLE[c0]);
      result[olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c0 + 1]);
      // memcpy(result + olength - i - 4, DIGIT_TABLE + c1, 2);
      result[olength - i - 4] = static_cast<CharT>(DIGIT_TABLE[c1]);
      result[olength - i - 3] = static_cast<CharT>(DIGIT_TABLE[c1 + 1]);

      i += 4;
    }
    if (digits >= 100) {
      const uint32_t c = (digits % 100) << 1;
      digits /= 100;
      // memcpy(result + olength - i - 2, DIGIT_TABLE + c, 2);
      result[olength - i - 2] = static_cast<CharT>(DIGIT_TABLE[c]);
      result[olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c + 1]);

      i += 2;
    }
    if (digits >= 10) {
      const uint32_t c = digits << 1;
      // memcpy(result + olength - i - 2, DIGIT_TABLE + c, 2);
      result[olength - i - 2] = static_cast<CharT>(DIGIT_TABLE[c]);
      result[olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
    } else {
      result[0] = static_cast<CharT>('0' + digits);
    }
  }

  // Convert `digits` to a sequence of decimal digits. Print the first digit, followed by a decimal
  // dot '.' followed by the remaining digits. The caller has to guarantee that:
  //   10^(olength-1) <= digits < 10^olength
  // e.g., by passing `olength` as `decimalLength9(digits)`.
  template <typename CharT>
  constexpr void
      append_d_digits(const uint32_t olength, uint32_t digits, CharT* const result) noexcept {

    uint32_t i = 0;
    while (digits >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
      const uint32_t c = digits - 10000 * (digits / 10000);
#else
      const uint32_t c = digits % 10000;
#endif
      digits /= 10000;
      const uint32_t c0 = (c % 100) << 1;
      const uint32_t c1 = (c / 100) << 1;
      // memcpy(result + olength + 1 - i - 2, DIGIT_TABLE + c0, 2);
      result[olength + 1 - i - 2] = static_cast<CharT>(DIGIT_TABLE[c0]);
      result[olength + 1 - i - 1] = static_cast<CharT>(DIGIT_TABLE[c0 + 1]);
      // memcpy(result + olength + 1 - i - 4, DIGIT_TABLE + c1, 2);
      result[olength + 1 - i - 4] = static_cast<CharT>(DIGIT_TABLE[c1]);
      result[olength + 1 - i - 3] = static_cast<CharT>(DIGIT_TABLE[c1 + 1]);
      i += 4;
    }
    if (digits >= 100) {
      const uint32_t c = (digits % 100) << 1;
      digits /= 100;
      // memcpy(result + olength + 1 - i - 2, DIGIT_TABLE + c, 2);
      result[olength + 1 - i - 2] = static_cast<CharT>(DIGIT_TABLE[c]);
      result[olength + 1 - i - 1] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
      i += 2;
    }
    if (digits >= 10) {
      const uint32_t c = digits << 1;
      result[2] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
      result[1] = static_cast<CharT>('.');
      result[0] = static_cast<CharT>(DIGIT_TABLE[c]);
    } else {
      result[1] = static_cast<CharT>('.');
      result[0] = static_cast<CharT>('0' + digits);
    }
  }

  // Convert `digits` to decimal and write the last `count` decimal digits to result.
  // If `digits` contains additional digits, then those are silently ignored.
  template <typename CharT>
  constexpr void
      append_c_digits(const uint32_t count, uint32_t digits, CharT* const result) noexcept {
    // Copy pairs of digits from DIGIT_TABLE.
    uint32_t i = 0;
    for (; i < count - 1; i += 2) {
      const uint32_t c = (digits % 100) << 1;
      digits /= 100;
      // memcpy(result + count - i - 2, DIGIT_TABLE + c, 2);
      result[count - i - 2] = static_cast<CharT>(DIGIT_TABLE[c]);
      result[count - i - 1] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
    }
    // Generate the last digit if count is odd.
    if (i < count) {
      const char c = static_cast<CharT>('0' + (digits % 10));
      result[count - i - 1] = c;
    }
  }

  template <typename CharT>
  constexpr void append_nine_digits(uint32_t digits, CharT* const result) noexcept {

    if (digits == 0) {
      // memset(result, '0', 9);
      result[0] = static_cast<CharT>('0');
      result[1] = static_cast<CharT>('0');
      result[2] = static_cast<CharT>('0');
      result[3] = static_cast<CharT>('0');
      result[4] = static_cast<CharT>('0');
      result[5] = static_cast<CharT>('0');
      result[6] = static_cast<CharT>('0');
      result[7] = static_cast<CharT>('0');
      result[8] = static_cast<CharT>('0');
      return;
    }

    for (uint32_t i = 0; i < 5; i += 4) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
      const uint32_t c = digits - 10000 * (digits / 10000);
#else
      const uint32_t c = digits % 10000;
#endif
      digits /= 10000;
      const uint32_t c0 = (c % 100) << 1;
      const uint32_t c1 = (c / 100) << 1;
      // memcpy(result + 7 - i, DIGIT_TABLE + c0, 2);
      result[7 - i] = static_cast<CharT>(DIGIT_TABLE[c0]);
      result[8 - i] = static_cast<CharT>(DIGIT_TABLE[c0 + 1]);
      // memcpy(result + 5 - i, DIGIT_TABLE + c1, 2);
      result[5 - i] = static_cast<CharT>(DIGIT_TABLE[c1]);
      result[6 - i] = static_cast<CharT>(DIGIT_TABLE[c1 + 1]);
    }
    result[0] = static_cast<CharT>('0' + digits);
  }

  constexpr uint32_t indexForExponent(const uint32_t e) noexcept { return (e + 15) / 16; }

  constexpr uint32_t pow10BitsForIndex(const uint32_t idx) noexcept {
    return 16 * idx + POW10_ADDITIONAL_BITS;
  }

  constexpr uint32_t lengthForIndex(const uint32_t idx) noexcept {
    // +1 for ceil, +16 for mantissa, +8 to round up when dividing by 9
    return (log10Pow2(16 * (int32_t)idx) + 1 + 16 + 8) / 9;
  }
  template <typename CharT>
  constexpr unsigned copy_special_str_printf(CharT* const   result,
                                             const bool     sign,
                                             const uint64_t mantissa) noexcept {
#if defined(_MSC_VER)
    // TODO: Check that -nan is expected output on Windows.
    if (sign) {
      result[0] = static_cast<CharT>('-');
    }
    if (mantissa) {
      if (mantissa < (1ull << (double_mantissa_bits - 1))) {
        // memcpy(result + sign, "nan(snan)", 9);
        result[sign + 0] = static_cast<CharT>('n');
        result[sign + 1] = static_cast<CharT>('a');
        result[sign + 2] = static_cast<CharT>('n');
        result[sign + 3] = static_cast<CharT>('(');
        result[sign + 4] = static_cast<CharT>('s');
        result[sign + 5] = static_cast<CharT>('n');
        result[sign + 6] = static_cast<CharT>('a');
        result[sign + 7] = static_cast<CharT>('n');
        result[sign + 8] = static_cast<CharT>(')');
        return sign + 9;
      }
      // memcpy(result + sign, "nan", 3);
      result[sign + 0] = static_cast<CharT>('n');
      result[sign + 1] = static_cast<CharT>('a');
      result[sign + 2] = static_cast<CharT>('n');
      return sign + 3;
    }
#else
    if (mantissa) {
      // memcpy(result, "nan", 3);
      result[sign + 0] = static_cast<CharT>('n');
      result[sign + 1] = static_cast<CharT>('a');
      result[sign + 2] = static_cast<CharT>('n');
      return 3;
    }
    if (sign) {
      result[0] = '-';
    }
#endif
    // memcpy(result + sign, "Infinity", 8);
    result[sign + 0] = static_cast<CharT>('I');
    result[sign + 1] = static_cast<CharT>('n');
    result[sign + 2] = static_cast<CharT>('f');
    result[sign + 3] = static_cast<CharT>('i');
    result[sign + 4] = static_cast<CharT>('n');
    result[sign + 5] = static_cast<CharT>('i');
    result[sign + 6] = static_cast<CharT>('t');
    result[sign + 7] = static_cast<CharT>('y');
    return sign + 8;
  }

  template <typename CharT>
  constexpr unsigned d2fixed_buffered_n(double             d,
                                        uint32_t           precision,
                                        CharT*             result,
                                        DoubleCastFunction bit_cast) noexcept {
    const uint64_t bits = bit_cast(d);

    // Decode bits into sign, mantissa, and exponent.
    const bool     ieeeSign = ((bits >> (double_mantissa_bits + double_exponent_bits)) & 1) != 0;
    const uint64_t ieeeMantissa = bits & ((1ull << double_mantissa_bits) - 1);
    const uint32_t ieeeExponent =
        static_cast<uint32_t>((bits >> double_mantissa_bits) & ((1u << double_exponent_bits) - 1));

    // Case distinction; exit early for the easy cases.
    if (ieeeExponent == ((1u << double_exponent_bits) - 1u)) {
      return copy_special_str_printf(result, ieeeSign, ieeeMantissa);
    }
    if (ieeeExponent == 0 && ieeeMantissa == 0) {
      int index = 0;
      if (ieeeSign) {
        result[index++] = '-';
      }
      result[index++] = '0';
      if (precision > 0) {
        result[index++] = '.';
        // memset(result + index, '0', precision);
        for (int i = 0; i < static_cast<int>(precision); ++i) {
          result[index + i] = static_cast<CharT>('0');
        }
        index += static_cast<int>(precision);
      }
      return index;
    }

    int32_t  e2{0};
    uint64_t m2{0};
    if (ieeeExponent == 0) {
      e2 = 1 - double_bias - double_mantissa_bits;
      m2 = ieeeMantissa;
    } else {
      e2 = (int32_t)ieeeExponent - double_bias - double_mantissa_bits;
      m2 = (1ull << double_mantissa_bits) | ieeeMantissa;
    }

    int  index = 0;
    bool nonzero = false;
    if (ieeeSign) {
      result[index++] = '-';
    }
    if (e2 >= -52) {
      const uint32_t idx = e2 < 0 ? 0 : indexForExponent((uint32_t)e2);
      const uint32_t p10bits = pow10BitsForIndex(idx);
      const int32_t  len = static_cast<int32_t>(lengthForIndex(idx));

      for (int32_t i = len - 1; i >= 0; --i) {
        const uint32_t j = p10bits - e2;
        // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above,
        // which is a slightly faster code path in mulShift_mod1e9. Instead, we can just increase
        // the multipliers.
        const uint32_t digits =
            mulShift_mod1e9(m2 << 8, POW10_SPLIT[POW10_OFFSET[idx] + i], (int32_t)(j + 8));
        if (nonzero) {
          append_nine_digits(digits, result + index);
          index += 9;
        } else if (digits != 0) {
          const uint32_t olength = decimalLength9(digits);
          append_n_digits(olength, digits, result + index);
          index += static_cast<int>(olength);
          nonzero = true;
        }
      }
    }
    if (!nonzero) {
      result[index++] = '0';
    }
    if (precision > 0) {
      result[index++] = '.';
    }

    if (e2 < 0) {
      const int32_t idx = -e2 / 16;

      const uint32_t blocks = precision / 9 + 1;
      // 0 = don't round up; 1 = round up unconditionally; 2 = round up if odd.
      int      roundUp = 0;
      uint32_t i = 0;
      if (blocks <= MIN_BLOCK_2[idx]) {
        i = blocks;
        // memset(result + index, '0', precision);
        for (int i = 0; i < static_cast<int>(precision); ++i) {
          result[index + i] = static_cast<CharT>('0');
        }
        index += static_cast<int>(precision);
      } else if (i < MIN_BLOCK_2[idx]) {
        i = MIN_BLOCK_2[idx];
        // memset(result + index, '0', 9 * i);
        for (int idx = 0; idx < static_cast<int>(9 * i); ++idx) {
          result[index + idx] = static_cast<CharT>('0');
        }
        index += static_cast<int>(9 * i);
      }
      for (; i < blocks; ++i) {
        const int32_t  j = ADDITIONAL_BITS_2 + (-e2 - 16 * idx);
        const uint32_t p = POW10_OFFSET_2[idx] + i - MIN_BLOCK_2[idx];
        if (p >= POW10_OFFSET_2[idx + 1]) {
          // If the remaining digits are all 0, then we might as well use memset.
          // No rounding required in this case.
          const uint32_t fill = precision - 9 * i;
          // memset(result + index, '0', fill);
          for (int i = 0; i < static_cast<int>(fill); ++i) {
            result[index + i] = static_cast<CharT>('0');
          }
          index += static_cast<int>(fill);
          break;
        }
        // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above,
        // which is a slightly faster code path in mulShift_mod1e9. Instead, we can just increase
        // the multipliers.
        uint32_t digits = mulShift_mod1e9(m2 << 8, POW10_SPLIT_2[p], j + 8);

        if (i < blocks - 1) {
          append_nine_digits(digits, result + index);
          index += 9;
        } else {
          const uint32_t maximum = precision - 9 * i;
          uint32_t       lastDigit = 0;
          for (uint32_t k = 0; k < 9 - maximum; ++k) {
            lastDigit = digits % 10;
            digits /= 10;
          }

          if (lastDigit != 5) {
            roundUp = lastDigit > 5;
          } else {
            // Is m * 10^(additionalDigits + 1) / 2^(-e2) integer?
            const int32_t requiredTwos = -e2 - (int32_t)precision - 1;
            const bool    trailingZeros =
                requiredTwos <= 0 ||
                (requiredTwos < 60 && multipleOfPowerOf2(m2, (uint32_t)requiredTwos));
            roundUp = trailingZeros ? 2 : 1;
          }
          if (maximum > 0) {
            append_c_digits(maximum, digits, result + index);
            index += static_cast<int>(maximum);
          }
          break;
        }
      }

      if (roundUp != 0) {
        int roundIndex = index;
        int dotIndex = 0; // '.' can't be located at index 0
        while (true) {
          --roundIndex;
          char c{0};
          if (roundIndex == -1 || (c = result[roundIndex], c == '-')) {
            result[roundIndex + 1] = '1';
            if (dotIndex > 0) {
              result[dotIndex] = '0';
              result[dotIndex + 1] = '.';
            }
            result[index++] = '0';
            break;
          }
          if (c == '.') {
            dotIndex = roundIndex;
            continue;
          } else if (c == '9') {
            result[roundIndex] = '0';
            roundUp = 1;
            continue;
          } else {
            if (roundUp == 2 && c % 2 == 0) {
              break;
            }
            result[roundIndex] = c + 1;
            break;
          }
        }
      }
    } else {
      // memset(result + index, '0', precision);
      for (uint32_t i = 0; i < precision; ++i) {
        result[index + i] = static_cast<CharT>('0');
      }
      index += static_cast<int>(precision);
    }
    return index;
  }

  template <typename CharT>
  constexpr unsigned d2exp_buffered_n(double             d,
                                      uint32_t           precision,
                                      CharT*             result,
                                      DoubleCastFunction bit_cast) noexcept {
    const uint64_t bits = bit_cast(d);

    // Decode bits into sign, mantissa, and exponent.
    const bool     ieeeSign = ((bits >> (double_mantissa_bits + double_exponent_bits)) & 1) != 0;
    const uint64_t ieeeMantissa = bits & ((1ull << double_mantissa_bits) - 1);
    const uint32_t ieeeExponent =
        (uint32_t)((bits >> double_mantissa_bits) & ((1u << double_exponent_bits) - 1));

    // Case distinction; exit early for the easy cases.
    if (ieeeExponent == ((1u << double_exponent_bits) - 1u)) {
      return copy_special_str_printf(result, ieeeSign, ieeeMantissa);
    }
    if (ieeeExponent == 0 && ieeeMantissa == 0) {
      int index = 0;
      if (ieeeSign) {
        result[index++] = '-';
      }
      result[index++] = '0';
      if (precision > 0) {
        result[index++] = '.';
        for (uint32_t i = 0; i < precision; ++i) {
          result[index + i] = static_cast<CharT>('0');
        }
        index += precision;
      }
      // memcpy(result + index, "e+00", 4);
      result[index] = 'e';
      result[index + 1] = '+';
      result[index + 2] = '0';
      result[index + 3] = '0';
      index += 4;
      return index;
    }

    int32_t  e2{0};
    uint64_t m2{0};
    if (ieeeExponent == 0) {
      e2 = 1 - double_bias - double_mantissa_bits;
      m2 = ieeeMantissa;
    } else {
      e2 = (int32_t)ieeeExponent - double_bias - double_mantissa_bits;
      m2 = (1ull << double_mantissa_bits) | ieeeMantissa;
    }

    const bool printDecimalPoint = precision > 0;
    ++precision;
    int index = 0;
    if (ieeeSign) {
      result[index++] = '-';
    }
    uint32_t digits = 0;
    uint32_t printedDigits = 0;
    uint32_t availableDigits = 0;
    int32_t  exp = 0;
    if (e2 >= -52) {
      const uint32_t idx = e2 < 0 ? 0 : indexForExponent((uint32_t)e2);
      const uint32_t p10bits = pow10BitsForIndex(idx);
      const int32_t  len = (int32_t)lengthForIndex(idx);

      for (int32_t i = len - 1; i >= 0; --i) {
        const uint32_t j = p10bits - e2;
        // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above,
        // which is a slightly faster code path in mulShift_mod1e9. Instead, we can just increase
        // the multipliers.
        digits = mulShift_mod1e9(m2 << 8, POW10_SPLIT[POW10_OFFSET[idx] + i], (int32_t)(j + 8));
        if (printedDigits != 0) {
          if (printedDigits + 9 > precision) {
            availableDigits = 9;
            break;
          }
          append_nine_digits(digits, result + index);
          index += 9;
          printedDigits += 9;
        } else if (digits != 0) {
          availableDigits = decimalLength9(digits);
          exp = i * 9 + (int32_t)availableDigits - 1;
          if (availableDigits > precision) {
            break;
          }
          if (printDecimalPoint) {
            append_d_digits(availableDigits, digits, result + index);
            index += availableDigits + 1; // +1 for decimal point
          } else {
            result[index++] = static_cast<CharT>('0' + digits);
          }
          printedDigits = availableDigits;
          availableDigits = 0;
        }
      }
    }

    if (e2 < 0 && availableDigits == 0) {
      const int32_t idx = -e2 / 16;

      for (int32_t i = MIN_BLOCK_2[idx]; i < 200; ++i) {
        const int32_t  j = ADDITIONAL_BITS_2 + (-e2 - 16 * idx);
        const uint32_t p = POW10_OFFSET_2[idx] + (uint32_t)i - MIN_BLOCK_2[idx];
        // Temporary: j is usually around 128, and by shifting a bit, we push it to 128 or above,
        // which is a slightly faster code path in mulShift_mod1e9. Instead, we can just increase
        // the multipliers.
        digits =
            (p >= POW10_OFFSET_2[idx + 1]) ? 0 : mulShift_mod1e9(m2 << 8, POW10_SPLIT_2[p], j + 8);

        if (printedDigits != 0) {
          if (printedDigits + 9 > precision) {
            availableDigits = 9;
            break;
          }
          append_nine_digits(digits, result + index);
          index += 9;
          printedDigits += 9;
        } else if (digits != 0) {
          availableDigits = decimalLength9(digits);
          exp = -(i + 1) * 9 + (int32_t)availableDigits - 1;
          if (availableDigits > precision) {
            break;
          }
          if (printDecimalPoint) {
            append_d_digits(availableDigits, digits, result + index);
            index += availableDigits + 1; // +1 for decimal point
          } else {
            result[index++] = static_cast<CharT>('0' + digits);
          }
          printedDigits = availableDigits;
          availableDigits = 0;
        }
      }
    }

    const uint32_t maximum = precision - printedDigits;
    if (availableDigits == 0) {
      digits = 0;
    }
    uint32_t lastDigit = 0;
    if (availableDigits > maximum) {
      for (uint32_t k = 0; k < availableDigits - maximum; ++k) {
        lastDigit = digits % 10;
        digits /= 10;
      }
    }

    // 0 = don't round up; 1 = round up unconditionally; 2 = round up if odd.
    int roundUp = 0;
    if (lastDigit != 5) {
      roundUp = lastDigit > 5;
    } else {
      // Is m * 2^e2 * 10^(precision + 1 - exp) integer?
      // precision was already increased by 1, so we don't need to write + 1 here.
      const int32_t rexp = (int32_t)precision - exp;
      const int32_t requiredTwos = -e2 - rexp;
      bool          trailingZeros = requiredTwos <= 0 ||
                           (requiredTwos < 60 && multipleOfPowerOf2(m2, (uint32_t)requiredTwos));
      if (rexp < 0) {
        const int32_t requiredFives = -rexp;
        trailingZeros = trailingZeros && multipleOfPowerOf5(m2, (uint32_t)requiredFives);
      }
      roundUp = trailingZeros ? 2 : 1;
    }
    if (printedDigits != 0) {
      if (digits == 0) {
        // memset(result + index, '0', maximum);
        for (uint32_t i = 0; i < maximum; ++i) {
          result[index + i] = '0';
        }
      } else {
        append_c_digits(maximum, digits, result + index);
      }
      index += static_cast<int>(maximum);
    } else {
      if (printDecimalPoint) {
        append_d_digits(maximum, digits, result + index);
        index += static_cast<int>(maximum + 1); // +1 for decimal point
      } else {
        result[index++] = static_cast<CharT>('0' + digits);
      }
    }

    if (roundUp != 0) {
      int roundIndex = index;
      while (true) {
        --roundIndex;
        char c{0};
        if (roundIndex == -1 || (c = result[roundIndex], c == '-')) {
          result[roundIndex + 1] = static_cast<CharT>('1');
          ++exp;
          break;
        }
        if (c == '.') {
          continue;
        } else if (c == '9') {
          result[roundIndex] = static_cast<CharT>('0');
          roundUp = 1;
          continue;
        } else {
          if (roundUp == 2 && c % 2 == 0) {
            break;
          }
          result[roundIndex] = c + 1;
          break;
        }
      }
    }
    result[index++] = static_cast<CharT>('e');
    if (exp < 0) {
      result[index++] = static_cast<CharT>('-');
      exp = -exp;
    } else {
      result[index++] = static_cast<CharT>('+');
    }

    if (exp >= 100) {
      const int32_t c = exp % 10;
      // memcpy(result + index, DIGIT_TABLE + 2 * (exp / 10), 2);
      result[index + 0] = static_cast<CharT>(DIGIT_TABLE[2 * (exp / 10)]);
      result[index + 1] = static_cast<CharT>(DIGIT_TABLE[2 * (exp / 10) + 1]);
      result[index + 2] = static_cast<CharT>('0' + c);
      index += 3;
    } else {
      // memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
      result[index + 0] = static_cast<CharT>(DIGIT_TABLE[2 * exp]);
      result[index + 1] = static_cast<CharT>(DIGIT_TABLE[2 * exp + 1]);
      index += 2;
    }

    return index;
  }

} // namespace ryu::detail

namespace ryu {
  template <typename CharT>
  inline unsigned d2fixed_buffered_n(double d, uint32_t precision, CharT* result) noexcept {
    return detail::d2fixed_buffered_n(d, precision, result, &to_bits);
  }

  template <typename CharT>
  inline unsigned d2exp_buffered_n(double d, uint32_t precision, CharT* result) noexcept {
    return detail::d2exp_buffered_n(d, precision, result, to_bits);
  }

  namespace cx {
    template <typename CharT>
    constexpr unsigned d2exp_buffered_n(double d, uint32_t precision, CharT* result) noexcept {
      return ryu::detail::d2exp_buffered_n(d, precision, result, cx::to_bits);
    }
    template <typename CharT>
    constexpr unsigned d2fixed_buffered_n(double d, uint32_t precision, CharT* result) noexcept {
      return ryu::detail::d2fixed_buffered_n(d, precision, result, &cx::to_bits);
    }
  } // namespace cx
} // namespace ryu

#endif /* RYU_IMPL_D2FIXED_IMPL_HPP */
