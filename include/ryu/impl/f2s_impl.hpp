#ifndef RYU_IMPL_F2S_IMPL_HPP
#define RYU_IMPL_F2S_IMPL_HPP
#include "ryu/common.hpp"
#include "ryu/digit_table.hpp"
#include "ryu/f2s.hpp"
#include "ryu/f2s_intrinsics.hpp"

namespace ryu::detail {

  template <typename CharT>
  constexpr unsigned
      to_chars(const floating_decimal_32 v, const bool sign, CharT* const result) noexcept {
    // Step 5: Print the decimal representation.
    unsigned index = 0;
    if (sign) {
      result[index++] = static_cast<CharT>('-');
    }

    uint32_t       output = v.mantissa;
    const uint32_t olength = decimalLength9(output);

    // Print the decimal digits.
    // The following code is equivalent to:
    // for (uint32_t i = 0; i < olength - 1; ++i) {
    //   const uint32_t c = output % 10; output /= 10;
    //   result[index + olength - i] = (char) ('0' + c);
    // }
    // result[index] = '0' + output % 10;
    uint32_t i = 0;
    while (output >= 10000) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=38217
      const uint32_t c = output - 10000 * (output / 10000);
#else
      const uint32_t c = output % 10000;
#endif
      output /= 10000;
      const uint32_t c0 = (c % 100) << 1;
      const uint32_t c1 = (c / 100) << 1;
      // memcpy(result + index + olength - i - 1, DIGIT_TABLE + c0, 2);
      result[index + olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c0]);
      result[index + olength - i] = static_cast<CharT>(DIGIT_TABLE[c0 + 1]);
      // memcpy(result + index + olength - i - 3, DIGIT_TABLE + c1, 2);
      result[index + olength - i - 3] = static_cast<CharT>(DIGIT_TABLE[c1]);
      result[index + olength - i - 2] = static_cast<CharT>(DIGIT_TABLE[c1 + 1]);
      i += 4;
    }
    if (output >= 100) {
      const uint32_t c = (output % 100) << 1;
      output /= 100;
      // memcpy(result + index + olength - i - 1, DIGIT_TABLE + c, 2);
      result[index + olength - i - 1] = static_cast<CharT>(DIGIT_TABLE[c]);
      result[index + olength - i] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
      i += 2;
    }
    if (output >= 10) {
      const uint32_t c = output << 1;
      // We can't use memcpy here: the decimal dot goes between these two digits.
      result[index + olength - i] = static_cast<CharT>(DIGIT_TABLE[c + 1]);
      result[index] = DIGIT_TABLE[c];
    } else {
      result[index] = static_cast<CharT>('0' + output);
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

    if (exp >= 10) {
      // memcpy(result + index, DIGIT_TABLE + 2 * exp, 2);
      const auto digit_idx = 2 * exp;
      result[index] = static_cast<CharT>(DIGIT_TABLE[digit_idx]);
      result[index + 1] = static_cast<CharT>(DIGIT_TABLE[digit_idx + 1]);
      index += 2;
    } else {
      result[index++] = static_cast<CharT>('0' + exp);
    }

    return index;
  }

  constexpr floating_decimal_32 f2d(const uint32_t ieeeMantissa,
                                    const uint32_t ieeeExponent) noexcept {
    int32_t  e2{0};
    uint32_t m2{0};
    if (ieeeExponent == 0) {
      // We subtract 2 so that the bounds computation has 2 additional bits.
      e2 = 1 - float_bias - float_mantissa_bits - 2;
      m2 = ieeeMantissa;
    } else {
      e2 = (int32_t)ieeeExponent - float_bias - float_mantissa_bits - 2;
      m2 = (1u << float_mantissa_bits) | ieeeMantissa;
    }
    const bool even = (m2 & 1) == 0;
    const bool acceptBounds = even;

    // Step 2: Determine the interval of valid decimal representations.
    const uint32_t mv = 4 * m2;
    const uint32_t mp = 4 * m2 + 2;
    // Implicit bool -> int conversion. True is 1, false is 0.
    const uint32_t mmShift = ieeeMantissa != 0 || ieeeExponent <= 1;
    const uint32_t mm = 4 * m2 - 1 - mmShift;

    // Step 3: Convert to a decimal power base using 64-bit arithmetic.
    uint32_t vr{0};
    uint32_t vp{0};
    uint32_t vm{0};
    int32_t  e10{0};
    bool     vmIsTrailingZeros = false;
    bool     vrIsTrailingZeros = false;
    uint8_t  lastRemovedDigit = 0;
    if (e2 >= 0) {
      const uint32_t q = log10Pow2(e2);
      e10 = (int32_t)q;
      const int32_t k = detail::float_pow5_inv_bitcount + pow5bits((int32_t)q) - 1;
      const int32_t i = -e2 + (int32_t)q + k;
      vr = mulPow5InvDivPow2(mv, q, i);
      vp = mulPow5InvDivPow2(mp, q, i);
      vm = mulPow5InvDivPow2(mm, q, i);

      if (q != 0 && (vp - 1) / 10 <= vm / 10) {
        // We need to know one removed digit even if we are not going to loop below. We could use
        // q = X - 1 above, except that would require 33 bits for the result, and we've found that
        // 32-bit arithmetic is faster even on 64-bit machines.
        const int32_t l = detail::float_pow5_inv_bitcount + pow5bits((int32_t)(q - 1)) - 1;
        lastRemovedDigit = (uint8_t)(mulPow5InvDivPow2(mv, q - 1, -e2 + (int32_t)q - 1 + l) % 10);
      }
      if (q <= 9) {
        // The largest power of 5 that fits in 24 bits is 5^10, but q <= 9 seems to be safe as well.
        // Only one of mp, mv, and mm can be a multiple of 5, if any.
        if (mv % 5 == 0) {
          vrIsTrailingZeros = multipleOfPowerOf5_32(mv, q);
        } else if (acceptBounds) {
          vmIsTrailingZeros = multipleOfPowerOf5_32(mm, q);
        } else {
          vp -= multipleOfPowerOf5_32(mp, q);
        }
      }
    } else {
      const uint32_t q = log10Pow5(-e2);
      e10 = (int32_t)q + e2;
      const int32_t i = -e2 - (int32_t)q;
      const int32_t k = pow5bits(i) - detail::float_pow5_bitcount;
      int32_t       j = (int32_t)q - k;
      vr = mulPow5divPow2(mv, (uint32_t)i, j);
      vp = mulPow5divPow2(mp, (uint32_t)i, j);
      vm = mulPow5divPow2(mm, (uint32_t)i, j);

      if (q != 0 && (vp - 1) / 10 <= vm / 10) {
        j = (int32_t)q - 1 - (pow5bits(i + 1) - detail::float_pow5_bitcount);
        lastRemovedDigit = (uint8_t)(mulPow5divPow2(mv, (uint32_t)(i + 1), j) % 10);
      }
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
      } else if (q < 31) { // TODO(ulfjack): Use a tighter bound here.
        vrIsTrailingZeros = multipleOfPowerOf2_32(mv, q - 1);
      }
    }

    // Step 4: Find the shortest decimal representation in the interval of valid representations.
    int32_t  removed = 0;
    uint32_t output{0};
    if (vmIsTrailingZeros || vrIsTrailingZeros) {
      // General case, which happens rarely (~4.0%).
      while (vp / 10 > vm / 10) {
#ifdef __clang__ // https://bugs.llvm.org/show_bug.cgi?id=23106
        // The compiler does not realize that vm % 10 can be computed from vm / 10
        // as vm - (vm / 10) * 10.
        vmIsTrailingZeros &= vm - (vm / 10) * 10 == 0;
#else
        vmIsTrailingZeros &= vm % 10 == 0;
#endif
        vrIsTrailingZeros &= lastRemovedDigit == 0;
        lastRemovedDigit = (uint8_t)(vr % 10);
        vr /= 10;
        vp /= 10;
        vm /= 10;
        ++removed;
      }

      if (vmIsTrailingZeros) {
        while (vm % 10 == 0) {
          vrIsTrailingZeros &= lastRemovedDigit == 0;
          lastRemovedDigit = (uint8_t)(vr % 10);
          vr /= 10;
          vp /= 10;
          vm /= 10;
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
      // Specialized for the common case (~96.0%). Percentages below are relative to this.
      // Loop iterations below (approximately):
      // 0: 13.6%, 1: 70.7%, 2: 14.1%, 3: 1.39%, 4: 0.14%, 5+: 0.01%
      while (vp / 10 > vm / 10) {
        lastRemovedDigit = (uint8_t)(vr % 10);
        vr /= 10;
        vp /= 10;
        vm /= 10;
        ++removed;
      }

      // We need to take vr + 1 if vr is outside bounds or we need to round up.
      output = vr + (vr == vm || lastRemovedDigit >= 5);
    }
    const int32_t exp = e10 + removed;

    floating_decimal_32 fd{};
    fd.exponent = exp;
    fd.mantissa = output;
    return fd;
  }

  template <typename CharT>
  constexpr unsigned f2s_buffered_n(float f, CharT* result, FloatCastFunction bit_cast) noexcept {
    // Step 1: Decode the floating-point number, and unify normalized and subnormal cases.
    const uint32_t bits = bit_cast(f);

    // Decode bits into sign, mantissa, and exponent.
    const bool     ieeeSign = ((bits >> (float_mantissa_bits + float_exponent_bits)) & 1) != 0;
    const uint32_t ieeeMantissa = bits & ((1u << float_mantissa_bits) - 1);
    const uint32_t ieeeExponent = (bits >> float_mantissa_bits) & ((1u << float_exponent_bits) - 1);

    // Case distinction; exit early for the easy cases.
    if (ieeeExponent == ((1u << float_exponent_bits) - 1u) ||
        (ieeeExponent == 0 && ieeeMantissa == 0)) {
      return copy_special_str(result, ieeeSign, ieeeExponent, ieeeMantissa);
    }

    const floating_decimal_32 v = f2d(ieeeMantissa, ieeeExponent);
    return to_chars(v, ieeeSign, result);
  }

} // namespace ryu::detail

namespace ryu {

  template <typename CharT>
  inline unsigned f2s_buffered_n(float f, CharT* result) noexcept {
    return detail::f2s_buffered_n(f, result, &to_bits);
  }

  namespace cx {} // namespace cx
} // namespace ryu

#endif /* RYU_IMPL_F2S_IMPL_HPP */
