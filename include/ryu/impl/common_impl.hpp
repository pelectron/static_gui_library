// The contents of this file originate from the ryu project by Ulf Adams (specifically the c version
// of ryu), available at https://github.com/ulfjack/ryu.git. Changes made were merely to make the
// ryu algorithm c++17 constexpr compliant, the core of the original algorithm remains unchanged.
//
//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef RYU_IMPL_COMMON_IMPL_HPP
#define RYU_IMPL_COMMON_IMPL_HPP
#include "../common.hpp"

#include <cstring>

namespace ryu {
  namespace detail {
    // Returns the number of decimal digits in v, which must not contain more than 9 digits.
    constexpr uint32_t decimalLength9(const uint32_t v) noexcept {
      // Function precondition: v is not a 10-digit number.
      // (f2s: 9 digits are sufficient for round-tripping.)
      // (d2fixed: We print 9-digit blocks.)
      // assert(v < 1000000000);
      if (v >= 100000000) {
        return 9;
      }
      if (v >= 10000000) {
        return 8;
      }
      if (v >= 1000000) {
        return 7;
      }
      if (v >= 100000) {
        return 6;
      }
      if (v >= 10000) {
        return 5;
      }
      if (v >= 1000) {
        return 4;
      }
      if (v >= 100) {
        return 3;
      }
      if (v >= 10) {
        return 2;
      }
      return 1;
    }

    // Returns e == 0 ? 1 : [log_2(5^e)]; requires 0 <= e <= 3528.
    constexpr int32_t log2pow5(const int32_t e) noexcept {
      // This approximation works up to the point that the multiplication overflows at e = 3529.
      // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
      // than 2^9297.
      // assert(e >= 0);
      // assert(e <= 3528);
      return static_cast<int32_t>((static_cast<uint32_t>(e) * 1217359) >> 19);
    }

    // Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
    constexpr int32_t pow5bits(const int32_t e) noexcept {
      // This approximation works up to the point that the multiplication overflows at e = 3529.
      // If the multiplication were done in 64 bits, it would fail at 5^4004 which is just greater
      // than 2^9297.
      // assert(e >= 0);
      // assert(e <= 3528);
      return static_cast<int32_t>(((static_cast<uint32_t>(e) * 1217359) >> 19) + 1);
    }

    // Returns e == 0 ? 1 : ceil(log_2(5^e)); requires 0 <= e <= 3528.
    constexpr int32_t ceil_log2pow5(const int32_t e) noexcept { return log2pow5(e) + 1; }

    // Returns floor(log_10(2^e)); requires 0 <= e <= 1650.
    constexpr uint32_t log10Pow2(const int32_t e) noexcept {
      // The first value this approximation fails for is 2^1651 which is just greater than 10^297.
      // assert(e >= 0);
      // assert(e <= 1650);
      return (static_cast<uint32_t>(e) * 78913) >> 18;
    }

    // Returns floor(log_10(5^e)); requires 0 <= e <= 2620.
    constexpr uint32_t log10Pow5(const int32_t e) noexcept {
      // The first value this approximation fails for is 5^2621 which is just greater than 10^1832.
      // assert(e >= 0);
      // assert(e <= 2620);
      return (static_cast<uint32_t>(e) * 732923) >> 20;
    }

    template <typename CharT>
    constexpr unsigned copy_special_str(CharT* const result,
                                        const bool   sign,
                                        const bool   exponent,
                                        const bool   mantissa) noexcept {
      if (mantissa) {
        result[0] = static_cast<CharT>('N');
        result[1] = static_cast<CharT>('a');
        result[2] = static_cast<CharT>('N');
        return 3u;
      }
      if (sign) {
        result[0] = static_cast<CharT>('-');
      }
      if (exponent) {
        result[sign + 0] = static_cast<CharT>('I');
        result[sign + 1] = static_cast<CharT>('n');
        result[sign + 2] = static_cast<CharT>('f');
        result[sign + 3] = static_cast<CharT>('i');
        result[sign + 4] = static_cast<CharT>('n');
        result[sign + 5] = static_cast<CharT>('i');
        result[sign + 6] = static_cast<CharT>('t');
        result[sign + 7] = static_cast<CharT>('y');
        return sign + 8u;
      }
      result[sign + 0] = static_cast<CharT>('0');
      result[sign + 1] = static_cast<CharT>('E');
      result[sign + 2] = static_cast<CharT>('0');
      return sign + 3u;
    }
  } // namespace detail

  inline uint64_t to_bits(const double f) noexcept {
    static_assert(sizeof(double) == 8, "double must be 8 bytes in size");
    uint64_t bits{0};
    memcpy(&bits, &f, sizeof(double));
    return bits;
  }

  inline uint32_t to_bits(const float f) noexcept {
    static_assert(sizeof(float) == 4, "double must be 4 bytes in size");
    uint32_t bits{0};
    memcpy(&bits, &f, sizeof(float));
    return bits;
  }
  namespace cx {

    // Based on code from
    // https://graphics.stanford.edu/~seander/bithacks.html
    constexpr int count_leading_zeroes(uint64_t v) noexcept {
      constexpr char bit_position[64] = {
          0,  1,  2,  7,  3,  13, 8,  19, 4,  25, 14, 28, 9,  34, 20, 40, 5,  17, 26, 38, 15, 46,
          29, 48, 10, 31, 35, 54, 21, 50, 41, 57, 63, 6,  12, 18, 24, 27, 33, 39, 16, 37, 45, 47,
          30, 53, 49, 56, 62, 11, 23, 32, 36, 44, 52, 55, 61, 22, 43, 51, 60, 42, 59, 58};

      v |= v >> 1; // first round down to one less than a power of 2
      v |= v >> 2;
      v |= v >> 4;
      v |= v >> 8;
      v |= v >> 16;
      v |= v >> 32;
      v = (v >> 1) + 1;

      return 63 - bit_position[(v * 0x0218a392cd3d5dbf) >> 58]; // [3]
    }

    constexpr uint32_t to_bits(const float f) noexcept {
      if (f == 0.0f)
        return 0; // also matches -0.0f and gives wrong result
      if (f == ryu::f_infinity)
        return 0x7f800000;
      if (f == -ryu::f_infinity)
        return 0xff800000;
      if (f != f)          // NaN
        return 0x7fc00000; // This is my NaN...

      const bool sign = f < 0.0f;
      float      abs_f = sign ? -f : f;

      int exponent = 254;

      while (abs_f < 0x1p87f) { // 87 = 64 + 23
        abs_f *= 0x1p41f;
        exponent -= 41; // 41 = 64-23
      }

      uint64_t a = static_cast<uint64_t>(abs_f * 0x1p-64f);
      int      lz = count_leading_zeroes(a);
      exponent -= lz;

      if (exponent <= 0) {
        exponent = 0;
        lz = 8 - 1;
      }

      uint32_t significand = (a << (lz + 1)) >> (64 - 23); // [3]
      return (sign << 31) | (exponent << 23) | significand;
    }

    constexpr uint64_t to_bits(const double f) noexcept {
      if (f == 0.0)
        return 0; // also matches -0.0f and gives wrong result
      if (f == ryu::d_infinity)
        return 0x7FF0000000000000ULL;
      if (f == -ryu::d_infinity)
        return 0xFFF0000000000000ULL;
      if (f != f)                     // NaN
        return 0x7FF8000000000000ULL; // This is my NaN...

      const bool sign = f < 0.0;
      // need to scale abs_f, so that a static_cast to uint64_t contains all mantissa bits->abs_f
      // should be bewteen 2e53 and 2e64
      double abs_f = sign ? -f : f;
      int    exponent = 1086;
      if (abs_f < 0x1p53) {
        while (abs_f < 0x1p53) {
          abs_f *= 0x1p11;
          exponent -= 11;
        }
      } else {
        while (abs_f > 0x1p64) {
          abs_f *= 0x1p-11;
          exponent += 11;
        }
      }
      // abs_f *= 0x1p-64;
      const auto a = static_cast<uint64_t>(abs_f);
      int        lz = count_leading_zeroes(a);
      exponent -= lz;

      // if (exponent <= 0) {
      //   exponent = 0;
      //   lz = 11 - 1;
      // }

      uint64_t significand = (a << (lz + 1)) >> (64 - 52); // [3]
      return (static_cast<uint64_t>(sign) << 63) | (static_cast<uint64_t>(exponent) << 52) |
             significand;
    }
  } // namespace cx

} // namespace ryu

#endif /* RYU_IMPL_COMMON_IMPL_HPP */
