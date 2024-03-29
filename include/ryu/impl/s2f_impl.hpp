// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef RYU_IMPL_S2F_IMPL_HPP
#define RYU_IMPL_S2F_IMPL_HPP
#include "gcem.hpp"
#include "gcem_incl/pow.hpp"
#include "ryu/common.hpp"
#include "ryu/s2f.hpp"

#include <limits>

namespace ryu {

  namespace detail {
    static inline float int32Bits2Float(uint32_t bits) noexcept {
      float f;
      memcpy(&f, &bits, sizeof(float));
      return f;
    }

    using Log2Function = uint32_t (*)(const uint32_t) noexcept;

    using BitsToFloatFunction = float (*)(const uint32_t) noexcept;

    constexpr ryu::status s2f_n(const char*         buffer,
                                const int           len,
                                float*              result,
                                Log2Function        floor_log2_func,
                                BitsToFloatFunction bit_cast) noexcept {
      if (len == 0) {
        return status::input_too_short;
      }
      int      m10digits = 0;
      int      e10digits = 0;
      int      dotIndex = len;
      int      eIndex = len;
      uint32_t m10 = 0;
      int32_t  e10 = 0;
      bool     signedM = false;
      bool     signedE = false;
      int      i = 0;
      if (buffer[i] == '-') {
        signedM = true;
        i++;
      }
      for (; i < len; i++) {
        char c = buffer[i];
        if (c == '.') {
          if (dotIndex != len) {
            return status::malformed_input;
          }
          dotIndex = i;
          continue;
        }
        if ((c < '0') || (c > '9')) {
          break;
        }
        if (m10digits >= 9) {
          return status::input_too_long;
        }
        m10 = 10 * m10 + (c - '0');
        if (m10 != 0) {
          m10digits++;
        }
      }
      if (i < len && ((buffer[i] == 'e') || (buffer[i] == 'E'))) {
        eIndex = i;
        i++;
        if (i < len && ((buffer[i] == '-') || (buffer[i] == '+'))) {
          signedE = buffer[i] == '-';
          i++;
        }
        for (; i < len; i++) {
          char c = buffer[i];
          if ((c < '0') || (c > '9')) {
            return status::malformed_input;
          }
          if (e10digits > 3) {
            // TODO: Be more lenient. Return +/-Infinity or +/-0 instead.
            return status::input_too_long;
          }
          e10 = 10 * e10 + (c - '0');
          if (e10 != 0) {
            e10digits++;
          }
        }
      }
      if (i < len) {
        return status::malformed_input;
      }
      if (signedE) {
        e10 = -e10;
      }
      e10 -= dotIndex < eIndex ? eIndex - dotIndex - 1 : 0;
      if (m10 == 0) {
        *result = signedM ? -0.0f : 0.0f;
        return status::success;
      }

      if ((m10digits + e10 <= -46) || (m10 == 0)) {
        // Number is less than 1e-46, which should be rounded down to 0; return +/-0.0.
        uint32_t ieee = ((uint32_t)signedM) << (float_exponent_bits + float_mantissa_bits);
        *result = bit_cast(ieee);
        return status::success;
      }
      if (m10digits + e10 >= 40) {
        // Number is larger than 1e+39, which should be rounded to +/-Infinity.
        uint32_t ieee = (((uint32_t)signedM) << (float_exponent_bits + float_mantissa_bits)) |
                        (0xffu << float_mantissa_bits);
        *result = bit_cast(ieee);
        return status::success;
      }

      // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion
      // was exact (trailingZeros).
      int32_t  e2{0};
      uint32_t m2{0};
      bool     trailingZeros{false};
      if (e10 >= 0) {
        // The length of m * 10^e in bits is:
        //   log2(m10 * 10^e10) = log2(m10) + e10 log2(10) = log2(m10) + e10 + e10 * log2(5)
        //
        // We want to compute the float_mantissa_bits + 1 top-most bits (+1 for the implicit leading
        // one in IEEE format). We therefore choose a binary output exponent of
        //   log2(m10 * 10^e10) - (float_mantissa_bits + 1).
        //
        // We use floor(log2(5^e10)) so that we get at least this many bits; better to
        // have an additional bit than to not have enough bits.
        e2 = floor_log2_func(m10) + e10 + log2pow5(e10) - (float_mantissa_bits + 1);

        // We now compute [m10 * 10^e10 / 2^e2] = [m10 * 5^e10 / 2^(e2-e10)].
        // To that end, we use the FLOAT_POW5_SPLIT table.
        int j = e2 - e10 - ceil_log2pow5(e10) + detail::float_pow5_bitcount;
        // assert(j >= 0);
        m2 = mulPow5divPow2(m10, e10, j);

        // We also compute if the result is exact, i.e.,
        //   [m10 * 10^e10 / 2^e2] == m10 * 10^e10 / 2^e2.
        // This can only be the case if 2^e2 divides m10 * 10^e10, which in turn requires that the
        // largest power of 2 that divides m10 + e10 is greater than e2. If e2 is less than e10,
        // then the result must be exact. Otherwise we use the existing multipleOfPowerOf2 function.
        trailingZeros = e2 < e10 || (e2 - e10 < 32 && multipleOfPowerOf2_32(m10, e2 - e10));
      } else {
        e2 = floor_log2_func(m10) + e10 - ceil_log2pow5(-e10) - (float_mantissa_bits + 1);

        // We now compute [m10 * 10^e10 / 2^e2] = [m10 / (5^(-e10) 2^(e2-e10))].
        int j = e2 - e10 + ceil_log2pow5(-e10) - 1 + detail::float_pow5_inv_bitcount;
        m2 = mulPow5InvDivPow2(m10, -e10, j);

        // We also compute if the result is exact, i.e.,
        //   [m10 / (5^(-e10) 2^(e2-e10))] == m10 / (5^(-e10) 2^(e2-e10))
        //
        // If e2-e10 >= 0, we need to check whether (5^(-e10) 2^(e2-e10)) divides m10, which is the
        // case iff pow5(m10) >= -e10 AND pow2(m10) >= e2-e10.
        //
        // If e2-e10 < 0, we have actually computed [m10 * 2^(e10 e2) / 5^(-e10)] above,
        // and we need to check whether 5^(-e10) divides (m10 * 2^(e10-e2)), which is the case iff
        // pow5(m10 * 2^(e10-e2)) = pow5(m10) >= -e10.
        trailingZeros = (e2 < e10 || (e2 - e10 < 32 && multipleOfPowerOf2_32(m10, e2 - e10))) &&
                        multipleOfPowerOf5_32(m10, -e10);
      }

      // Compute the final IEEE exponent.
      uint32_t ieee_e2 = (uint32_t)max32(0, e2 + float_bias + floor_log2_func(m2));

      if (ieee_e2 > 0xfe) {
        // Final IEEE exponent is larger than the maximum representable; return +/-Infinity.
        uint32_t ieee = (((uint32_t)signedM) << (float_exponent_bits + float_mantissa_bits)) |
                        (0xffu << float_mantissa_bits);
        *result = bit_cast(ieee);
        return status::success;
      }

      // We need to figure out how much we need to shift m2. The tricky part is that we need to take
      // the final IEEE exponent into account, so we need to reverse the bias and also special-case
      // the value 0.
      int32_t shift = (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - float_bias - float_mantissa_bits;
      // assert(shift >= 0);

      // We need to round up if the exact value is more than 0.5 above the value we computed. That's
      // equivalent to checking if the last removed bit was 1 and either the value was not just
      // trailing zeros or the result would otherwise be odd.
      //
      // We need to update trailingZeros given that we have the exact output exponent ieee_e2 now.
      trailingZeros &= (m2 & ((1u << (shift - 1)) - 1)) == 0;
      uint32_t lastRemovedBit = (m2 >> (shift - 1)) & 1;
      bool     roundUp = (lastRemovedBit != 0) && (!trailingZeros || (((m2 >> shift) & 1) != 0));

      uint32_t ieee_m2 = (m2 >> shift) + roundUp;
      // assert(ieee_m2 <= (1u << (float_mantissa_bits + 1)));
      ieee_m2 &= (1u << float_mantissa_bits) - 1;
      if (ieee_m2 == 0 && roundUp) {
        // Rounding up may overflow the mantissa.
        // In this case we move a trailing zero of the mantissa into the exponent.
        // Due to how the IEEE represents +/-Infinity, we don't need to check for overflow here.
        ieee_e2++;
      }
      uint32_t ieee = (((((uint32_t)signedM) << float_exponent_bits) | (uint32_t)ieee_e2)
                       << float_mantissa_bits) |
                      ieee_m2;
      *result = bit_cast(ieee);
      return status::success;
    }

    constexpr uint32_t mask(uint32_t num_bits) noexcept {
      uint32_t ret{0};
      for (uint32_t i = 0; i < num_bits; ++i) {
        ret |= uint32_t{1} << i;
      }
      return ret;
    }

    static_assert(mask(1) == 1);
    static_assert(mask(2) == 0b11);
  } // namespace detail

  namespace cx {
    constexpr bool is_nan(uint32_t bits) {
      return ((0x7F800000u & bits) == 0x7F800000u) and ((bits & ~0xFF800000u) != 0);
    }

    constexpr float int32Bits2Float(uint32_t bits) noexcept {
      if (bits == 0)
        return 0.0f;
      if (ryu::cx::is_nan(bits))
        return std::numeric_limits<float>::quiet_NaN();
      if (bits == 0x80000000u)
        return -0.0f;
      if (bits == 0x7F800000u)
        return ryu::f_infinity;
      if (bits == 0xFF800000u)
        return -ryu::f_infinity;

      int sign = ((bits >> 31) == 1) ? -1 : 1;
      int exponent = ((bits >> ryu::float_mantissa_bits) & detail::mask(ryu::float_exponent_bits)) -
                     ryu::float_bias;

      uint32_t mantissa = bits & detail::mask(ryu::float_mantissa_bits);
      if (exponent == -127) {
        return sign * (mantissa / gcem::pow(2.0f, ryu::float_mantissa_bits)) *
               gcem::pow(2.0f, -126);
      }
      return sign * (1 + mantissa / gcem::pow(2.0f, ryu::float_mantissa_bits)) *
             gcem::pow(2.0f, exponent);
    }

    [[nodiscard]] constexpr status s2f_n(const char* buffer, const int len, float* result) noexcept {
      return ryu::detail::s2f_n(buffer,
                                len,
                                result,
                                &ryu::cx::floor_log2,
                                &ryu::cx::int32Bits2Float);
    }
  } // namespace cx

  inline float int32Bits2Float(uint32_t bits) noexcept {
    float f{0};
    memcpy(&f, &bits, 4);
    return f;
  }

  [[nodiscard]] status s2f_n(const char* buffer, const int len, float* result) noexcept {
    return ryu::detail::s2f_n(buffer, len, result, &ryu::detail::floor_log2, &int32Bits2Float);
  }
} // namespace ryu

#endif /* RYU_IMPL_S2F_IMPL_HPP */
