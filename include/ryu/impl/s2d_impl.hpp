// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef RYU_IMPL_S2D_IMPL_HPP
#define RYU_IMPL_S2D_IMPL_HPP
#include "ryu/common.hpp"
#include "ryu/d2s_full_table.hpp"
#include "ryu/d2s_intrinsics.hpp"
#include "ryu/impl/s2f_impl.hpp"

#include <limits>

namespace ryu {

  namespace detail {
    using DobuleCastFunction = double (*)(uint64_t);

    using Log2Function64 = uint32_t (*)(const uint64_t) noexcept;

    constexpr status s2d_n(const char*        buffer,
                           const int          len,
                           double*            result,
                           DobuleCastFunction bit_cast,
                           Log2Function64     floor_log2_func) {
      if (len == 0) {
        return status::input_too_short;
      }
      int      m10digits = 0;
      int      e10digits = 0;
      int      dotIndex = len;
      int      eIndex = len;
      uint64_t m10 = 0;
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
        if (m10digits >= 17) {
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
        *result = signedM ? -0.0 : 0.0;
        return status::success;
      }

#ifdef RYU_DEBUG
      printf("input=%s\n", buffer);
      printf("m10digits = %d\n", m10digits);
      printf("e10digits = %d\n", e10digits);
      printf("m10 * 10^e10 = %" PRIu64 " * 10^%d\n", m10, e10);
#endif

      if ((m10digits + e10 <= -324) || (m10 == 0)) {
        // Number is less than 1e-324, which should be rounded down to 0; return +/-0.0.
        uint64_t ieee = ((uint64_t)signedM)
                        << (ryu::double_exponent_bits + ryu::double_mantissa_bits);
        *result = bit_cast(ieee);
        return status::success;
      }
      if (m10digits + e10 >= 310) {
        // Number is larger than 1e+309, which should be rounded to +/-Infinity.
        uint64_t ieee =
            (((uint64_t)signedM) << (ryu::double_exponent_bits + ryu::double_mantissa_bits)) |
            (0x7ffull << ryu::double_mantissa_bits);
        *result = bit_cast(ieee);
        return status::success;
      }

      // Convert to binary float m2 * 2^e2, while retaining information about whether the conversion
      // was exact (trailingZeros).
      int32_t  e2{0};
      uint64_t m2{0};
      bool     trailingZeros{0};
      if (e10 >= 0) {
        // The length of m * 10^e in bits is:
        //   log2(m10 * 10^e10) = log2(m10) + e10 log2(10) = log2(m10) + e10 + e10 * log2(5)
        //
        // We want to compute the ryu::double_mantissa_bits + 1 top-most bits (+1 for the implicit
        // leading one in IEEE format). We therefore choose a binary output exponent of
        //   log2(m10 * 10^e10) - (ryu::double_mantissa_bits + 1).
        //
        // We use floor(log2(5^e10)) so that we get at least this many bits; better to
        // have an additional bit than to not have enough bits.
        e2 = floor_log2_func(m10) + e10 + log2pow5(e10) - (ryu::double_mantissa_bits + 1);

        // We now compute [m10 * 10^e10 / 2^e2] = [m10 * 5^e10 / 2^(e2-e10)].
        // To that end, we use the DOUBLE_POW5_SPLIT table.
        int j = e2 - e10 - ceil_log2pow5(e10) + ryu::config::DOUBLE_POW5_BITCOUNT;
        // assert(j >= 0);
#if defined(RYU_OPTIMIZE_SIZE)
        uint64_t pow5[2];
        double_computePow5(e10, pow5);
        m2 = mulShift64(m10, pow5, j);
#else
        // assert(e10 < DOUBLE_POW5_TABLE_SIZE);
        m2 = mulShift64(m10, ryu::detail::DOUBLE_POW5_SPLIT[e10], j);
#endif
        // We also compute if the result is exact, i.e.,
        //   [m10 * 10^e10 / 2^e2] == m10 * 10^e10 / 2^e2.
        // This can only be the case if 2^e2 divides m10 * 10^e10, which in turn requires that the
        // largest power of 2 that divides m10 + e10 is greater than e2. If e2 is less than e10,
        // then the result must be exact. Otherwise we use the existing multipleOfPowerOf2 function.
        trailingZeros =
            e2 < e10 || (e2 - e10 < 64 && ryu::detail::multipleOfPowerOf2(m10, e2 - e10));
      } else {
        e2 = floor_log2_func(m10) + e10 - ceil_log2pow5(-e10) - (ryu::double_mantissa_bits + 1);
        int j = e2 - e10 + ceil_log2pow5(-e10) - 1 + ryu::config::DOUBLE_POW5_INV_BITCOUNT;
#if defined(RYU_OPTIMIZE_SIZE)
        uint64_t pow5[2];
        double_computeInvPow5(-e10, pow5);
        m2 = mulShift64(m10, pow5, j);
#else
        // assert(-e10 < DOUBLE_POW5_INV_TABLE_SIZE);
        m2 = mulShift64(m10, DOUBLE_POW5_INV_SPLIT[-e10], j);
#endif
        trailingZeros = multipleOfPowerOf5(m10, -e10);
      }

      // Compute the final IEEE exponent.
      uint32_t ieee_e2 = (uint32_t)max32(0, e2 + ryu::double_bias + floor_log2_func(m2));

      if (ieee_e2 > 0x7fe) {
        // Final IEEE exponent is larger than the maximum representable; return +/-Infinity.
        uint64_t ieee =
            (((uint64_t)signedM) << (ryu::double_exponent_bits + ryu::double_mantissa_bits)) |
            (0x7ffull << ryu::double_mantissa_bits);
        *result = bit_cast(ieee);
        return status::success;
      }

      // We need to figure out how much we need to shift m2. The tricky part is that we need to take
      // the final IEEE exponent into account, so we need to reverse the bias and also special-case
      // the value 0.
      int32_t shift =
          (ieee_e2 == 0 ? 1 : ieee_e2) - e2 - ryu::double_bias - ryu::double_mantissa_bits;

      // We need to round up if the exact value is more than 0.5 above the value we computed. That's
      // equivalent to checking if the last removed bit was 1 and either the value was not just
      // trailing zeros or the result would otherwise be odd.
      //
      // We need to update trailingZeros given that we have the exact output exponent ieee_e2 now.
      trailingZeros &= (m2 & ((1ull << (shift - 1)) - 1)) == 0;
      uint64_t lastRemovedBit = (m2 >> (shift - 1)) & 1;
      bool     roundUp = (lastRemovedBit != 0) && (!trailingZeros || (((m2 >> shift) & 1) != 0));

      uint64_t ieee_m2 = (m2 >> shift) + roundUp;
      ieee_m2 &= (1ull << ryu::double_mantissa_bits) - 1;
      if (ieee_m2 == 0 && roundUp) {
        // Due to how the IEEE represents +/-Infinity, we don't need to check for overflow here.
        ieee_e2++;
      }

      uint64_t ieee = (((((uint64_t)signedM) << ryu::double_exponent_bits) | (uint64_t)ieee_e2)
                       << ryu::double_mantissa_bits) |
                      ieee_m2;
      *result = bit_cast(ieee);
      return status::success;
    }

    inline double bits_to_double(uint64_t bits) noexcept {
      double ret{};
      memcpy(&ret, &bits, sizeof(ret));
      return ret;
    }

  } // namespace detail

  namespace cx {
    constexpr bool is_nan(uint64_t bits) noexcept {
      return ((bits & 0x7FF0000000000000u) == 0x7FF0000000000000u) &&
             ((bits & ~0xFFF0000000000000u) != 0);
    }

    constexpr uint64_t mask64(int n) {
      uint64_t ret{0};
      for (int i = 0; i < n; ++i) {
        ret |= uint64_t{1} << i;
      }
      return ret;
    }

    constexpr double bits_to_double(uint64_t bits) noexcept {
      if (bits == 0)
        return 0.0;
      if (ryu::cx::is_nan(bits))
        return std::numeric_limits<double>::quiet_NaN();
      if (bits == 0x8000000000000000u)
        return -0.0f;
      if (bits == 0x7FF0000000000000u)
        return ryu::d_infinity;
      if (bits == 0xFFF0000000000000u)
        return -ryu::d_infinity;

      const int sign = ((bits >> 63) == 1) ? -1 : 1;
      const int e =
          static_cast<int>((bits >> ryu::double_mantissa_bits) & mask64(ryu::double_exponent_bits));
      const double mantissa = static_cast<double>(bits & mask64(ryu::double_mantissa_bits));
      if (e == 0) {
        // subnormal
        return sign * (mantissa / gcem::pow(2.0, ryu::double_mantissa_bits)) *
               gcem::pow(2.0, -1022);
      }
      return sign * (1.0 + mantissa / gcem::pow(2.0, ryu::double_mantissa_bits)) *
             gcem::pow(2.0, e - ryu::double_bias);
    }

    static_assert(ryu::cx::bits_to_double(uint64_t(0x3FF0000000000000u)) == 1.0);
    static_assert(ryu::cx::bits_to_double(uint64_t(0x4000000000000000u)) == 2.0);

    constexpr status s2d_n(const char* buffer, const int len, double* result) noexcept {
      return detail::s2d_n(buffer, len, result, &ryu::cx::bits_to_double, &ryu::cx::floor_log2);
    }

  } // namespace cx

  [[nodiscard]] inline status s2d_n(const char* buffer, const int len, double* result) noexcept {
    return detail::s2d_n(buffer, len, result, &ryu::detail::bits_to_double, &ryu::detail::floor_log2);
  }
} // namespace ryu
#endif // !RYU_IMPL_S2D_IMPL_HPP
