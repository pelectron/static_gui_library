//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
#ifndef SGL_IMPL_FORMAT_IMPL_HPP
#define SGL_IMPL_FORMAT_IMPL_HPP
#include "ryu/ryu.hpp"
#include "sgl/config.h"
#include "sgl/format.hpp"
#include "sgl/limits.hpp"
#include "sgl/static_string.hpp"

#include <gcem.hpp>
#include <type_traits>

namespace sgl {

  namespace format_impl {
    template <typename T>
    struct biggest_pow10_t;

    template <>
    struct biggest_pow10_t<uint8_t> {
      static constexpr size_t value = 100;
    };

    template <>
    struct biggest_pow10_t<int8_t> {
      static constexpr size_t value = 100;
    };

    template <>
    struct biggest_pow10_t<uint16_t> {
      static constexpr size_t value = 10000;
    };

    template <>
    struct biggest_pow10_t<int16_t> {
      static constexpr size_t value = 10000;
    };

    template <>
    struct biggest_pow10_t<uint32_t> {
      static constexpr size_t value = 1000000;
    };

    template <>
    struct biggest_pow10_t<int32_t> {
      static constexpr size_t value = 1000000;
    };

    template <>
    struct biggest_pow10_t<uint64_t> {
      static constexpr size_t value = 10000000000000000000ULL;
    };

    template <>
    struct biggest_pow10_t<int64_t> {
      static constexpr size_t value = 1000000000000000000;
    };

    template <typename T>
    static constexpr size_t biggest_pow10_v = biggest_pow10_t<T>::value;

    template <typename T>
    constexpr size_t biggest_pow10(T val) {
      // 18,446,744,073,709,551,616
      if constexpr (numeric_limits<T>::max() > 10000000000000000000ULL)
        if (val >= T{10000000000000000000ULL}) {
          return 10000000000000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 1000000000000000000ULL)
        if (val >= T{1000000000000000000ULL}) {
          return 1000000000000000000ULL;
        }

      if constexpr (numeric_limits<T>::max() > 100000000000000000ULL)
        if (val >= T{100000000000000000ULL}) {
          return 100000000000000000ULL;
        }

      if constexpr (numeric_limits<T>::max() > 10000000000000000ULL)
        if (val >= T{10000000000000000ULL}) {
          return 10000000000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 1000000000000000ULL)
        if (val >= T{1000000000000000ULL}) {
          return 1000000000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 100000000000000ULL)
        if (val >= T{100000000000000ULL}) {
          return 100000000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 10000000000000ULL)
        if (val >= T{10000000000000ULL}) {
          return 10000000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 1000000000000ULL)
        if (val >= T{1000000000000ULL}) {
          return 1000000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 100000000000ULL)
        if (val >= T{100000000000ULL}) {
          return 100000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 10000000000ULL)
        if (val >= T{10000000000ULL}) {
          return 10000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 1000000000ULL)
        if (val >= T{1000000000ULL}) {
          return 1000000000ULL;
        }
      if constexpr (numeric_limits<T>::max() > 100000000ULL)
        if (val >= T{100000000ULL}) {
          return 100000000;
        }
      if constexpr (numeric_limits<T>::max() > 10000000ULL)
        if (val >= T{10000000ULL}) {
          return 10000000;
        }
      if constexpr (numeric_limits<T>::max() > 1000000ULL)
        if (val >= T{1000000ULL}) {
          return 1000000;
        }
      if constexpr (numeric_limits<T>::max() > 100000ULL)
        if (val >= T{100000ULL}) {
          return 100000;
        }
      if constexpr (numeric_limits<T>::max() > 10000ULL)
        if (val >= T{10000ULL}) {
          return 10000;
        }
      if constexpr (numeric_limits<T>::max() > 1000ULL)
        if (val >= T{1000ULL}) {
          return 1000;
        }
      if (val >= T{100ULL}) {
        return 100;
      }
      if (val >= T{10ULL}) {
        return 10;
      }
      return 1;
    }

    template <typename T>
    struct max_buf_size;

    template <>
    struct max_buf_size<uint8_t> {
      static constexpr size_t value = 3;
    };

    template <>
    struct max_buf_size<int8_t> {
      static constexpr size_t value = 4;
    };

    template <>
    struct max_buf_size<uint16_t> {
      static constexpr size_t value = 5;
    };

    template <>
    struct max_buf_size<int16_t> {
      static constexpr size_t value = 6;
    };

    template <>
    struct max_buf_size<uint32_t> {
      static constexpr size_t value = 10;
    };

    template <>
    struct max_buf_size<int32_t> {
      static constexpr size_t value = 11;
    };

    template <>
    struct max_buf_size<uint64_t> {
      static constexpr size_t value = 20;
    };

    template <>
    struct max_buf_size<int64_t> {
      static constexpr size_t value = 21;
    };

    template <>
    struct max_buf_size<float> {
      static constexpr size_t value = 15;
    };

    template <>
    struct max_buf_size<double> {
      static constexpr size_t value = 24;
    };

    // NOLINTBEGIN(readability*)
    template <typename T>
    constexpr uint64_t biggest_pow16(T value) {
      if constexpr (numeric_limits<T>::max() >= 0x1000000000000000)
        if (value > T{0x1000000000000000}) {
          return 0x1000000000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x100000000000000)
        if (value > T{0x100000000000000}) {
          return 0x100000000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x10000000000000)
        if (value > T{0x10000000000000}) {
          return 0x10000000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x1000000000000)
        if (value > T{0x1000000000000}) {
          return 0x1000000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x100000000000)
        if (value > T{0x100000000000}) {
          return 0x100000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x10000000000)
        if (value > T{0x10000000000}) {
          return 0x10000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x1000000000)
        if (value > T{0x1000000000}) {
          return 0x1000000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x100000000)
        if (value > T{0x100000000}) {
          return 0x100000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x10000000)
        if (value > T{0x10000000}) {
          return 0x10000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x1000000)
        if (value > T{0x1000000}) {
          return 0x1000000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x100000)
        if (value > T{0x100000}) {
          return 0x100000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x10000)
        if (value > T{0x10000}) {
          return 0x10000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x1000)
        if (value > T{0x1000}) {
          return 0x1000;
        }
      if constexpr (numeric_limits<T>::max() >= 0x100)
        if (value > T{0x100}) {
          return 0x100;
        }
      if constexpr (numeric_limits<T>::max() >= 0x10)
        if (value > T{0x10}) {
          return 0x10;
        }
      return 0x1;
    }

    // NOLINTEND(readability-*)

    template <typename T>
    static constexpr size_t max_buf_size_v = sgl::format_impl::max_buf_size<T>::value;

    template <typename CharT, typename T>
    constexpr sgl::format_result basic_hex_format(CharT* str, size_t len, T value) {
      constexpr size_t base = 16;
      constexpr auto   hex_char = [](T val) -> CharT {
        if (val >= 0 && val < 10) {
          return static_cast<CharT>(val + '0');
        }
        if (val < static_cast<T>(16)) {
          return static_cast<CharT>(val + 'A' - 10);
        }
        return 0;
      };

      static_assert(std::is_integral_v<T>, "T must be an integral type");
      // static_assert(std::is_unsigned_v<T>, "");
      constexpr size_t           size = 2 * sizeof(T) + 3; // +2 for '0x', + 1 for '-'
      static_string<CharT, size> buf{};
      if constexpr (std::is_signed_v<T>) {
        if (value < 0) {
          buf.append(CharT{'-'});
          value = -value;
        }
      }
      buf.append("0x", 2);
      for (size_t pow16 = biggest_pow16(value); pow16 != 0; pow16 /= base) {
        buf.append(hex_char(static_cast<T>(value / pow16)));
        value = value % pow16;
      }
      if (buf.size() >= len) {
        return {sgl::error::format_error, 0};
      }
      for (const auto& ch : buf) {
        *str = ch;
        ++str;
      }
      return {sgl::error::no_error, buf.size()};
    }

    template <typename CharT, typename T>
    constexpr sgl::format_result basic_integer_format(CharT* str, size_t len, T value) {
      static_assert(std::is_integral_v<T>, "T must be an integral type");
      constexpr size_t                                          base = 10;
      static_string<CharT, sgl::format_impl::max_buf_size_v<T>> buf{};
      if constexpr (std::is_signed_v<T>) {
        if (value < 0) {
          buf.append(CharT{'-'});
          value = -value;
        }
      }
      for (size_t pow10 = biggest_pow10(value); pow10 != 0; pow10 /= base) {
        buf.append(static_cast<CharT>(value / pow10 + '0'));
        value = value % pow10;
      }
      if (buf.size() > len) {
        return {sgl::error::buffer_too_small, 0};
      }
      for (const auto& ch : buf) {
        *str = ch;
        ++str;
      }
      return {sgl::error::no_error, buf.size()};
    }

    template <typename CharT, typename T>
    constexpr sgl::format_result integer_format(CharT*      str,
                                                size_t      len,
                                                T           value,
                                                uint32_t    precision,
                                                sgl::format fmt) noexcept {
      static_assert(std::is_integral_v<T>, "T must be an integral type");
      (void)precision;
      switch (fmt) {
        case sgl::format::exponential:
          [[fallthrough]];
        case sgl::format::fixed:
        case sgl::format::floating:
        case sgl::format::integer:
          return basic_integer_format(str, len, value);
        case sgl::format::hex:
          return basic_hex_format(str, len, value);
        default:
          return {sgl::error::invalid_format, 0};
      }

      return {sgl::error::null_format, 0};
    }
  } // namespace format_impl

  template <typename CharT, typename T>
  sgl::error parse(const CharT* str, const size_t len, T& value) noexcept {
    if constexpr (std::is_same_v<double, T>) {
      static_assert(std::is_same_v<CharT, char>,
                    "only CharT=char supported with floating point parsing");

      if (ryu::s2d_n(str, len, &value) == ryu::status::success)
        return sgl::error::no_error;
      else
        return sgl::error::format_error;

    } else if constexpr (std::is_same_v<float, T>) {
      static_assert(std::is_same_v<CharT, char>,
                    "only CharT=char supported with floating point parsing");

      if (ryu::s2f_n(str, &value) == ryu::status::success)
        return sgl::error::no_error;
      else
        return sgl::error::format_error;

    } else {

      if ((len > sgl::format_impl::max_buf_size_v<T>) || (len == 0))
        return sgl::error::format_error;

      bool   negative = false;
      size_t i{0};
      if (str[0] == '-') {
        negative = true;
        i = 1;
      }
      T val{0};
      for (; i < len; ++i) {
        if ((str[i] >= '0') and (str[i] <= '9')) {
          val = val * 10 + (str[i] - '0');
        } else {
          return sgl::error::format_error;
        }
      }
      if (negative)
        val = -val;
      value = val;
      return sgl::error::no_error;
    }
  }

  namespace parse_impl {
    template <typename T>
    constexpr T pow10(int exp) {
      constexpr T base{10};
      T           f{1};
      if (exp < 0) {
        exp = -exp;
        while (exp--) {
          f /= base;
        }
      } else if (exp > 0) {
        while (exp--) {
          f *= base;
        }
      }
      return f;
    }

    static constexpr size_t not_found{sgl::numeric_limits<size_t>::max()};

    constexpr auto to_num(char c) -> int64_t { return (c - '0'); }

    template <typename T>
    constexpr T decimal(const char* begin, const char* end) {
      if (begin >= end) {
        return 0;
      }
      if (begin[0] == '-') {
        return -sgl::parse_impl::decimal<T>(begin + 1, end);
      }
      if (begin[0] == '+') {
        return sgl::parse_impl::decimal<T>(begin + 1, end);
      }
      T               result{0};
      const long long size{end - begin};

      for (long long i = 0; i < size; ++i) {
        result += to_num(begin[i]) * sgl::parse_impl::pow10<T>(static_cast<int>(size - i - 1));
      }
      return result;
    }

    constexpr auto get_dot_index(const char* str, size_t n) {
      for (size_t i = 0; i < n; ++i) {
        if (str[i] == '.') {
          return i;
        }
      }
      return sgl::parse_impl::not_found;
    }

    constexpr auto get_e_index(const char* str, size_t n) {
      for (size_t i = 0; i < n; ++i) {
        if ((str[i] == 'e') || (str[i] == 'E')) {
          return i;
        }
      }
      return sgl::parse_impl::not_found;
    }

    template <typename T>
    constexpr T parse(const char* str, size_t n) {
      if constexpr (std::is_integral_v<T>) {
        return sgl::parse_impl::decimal<T>(str, str + n);
      }
      size_t dot_index{get_dot_index(str, n)};
      size_t e_index{get_e_index(str, n)};
      if (e_index == not_found) {
        // no exponent -> DEC[.]FRAC
        if (dot_index == not_found) {
          // DEC
          return static_cast<T>(sgl::parse_impl::decimal<T>(str, str + n));
        }
        // [DEC].FRAC
        auto dec = sgl::parse_impl::decimal<T>(str, str + dot_index);
        auto frac = static_cast<T>(sgl::parse_impl::decimal<T>(str + dot_index + 1, str + n)) /
                    sgl::parse_impl::pow10<T>(static_cast<int>(n - dot_index - 1));
        return static_cast<T>(dec) + frac;
      }
      // has exponent
      // DEC[.][FRAC][Ee][+-]EXP
      if (dot_index == not_found) {
        // DEC[Ee][+-]EXP
        auto dec = sgl::parse_impl::decimal<T>(str, str + e_index);
        auto exp = sgl::parse_impl::decimal<T>(str + e_index + 1, str + n);
        return static_cast<T>(dec) * sgl::parse_impl::pow10<T>(static_cast<int>(exp));
      }
      // [DEC].FRAC[Ee][+-]EXP
      auto dec = sgl::parse_impl::decimal<T>(str, str + dot_index);
      auto frac = static_cast<T>(sgl::parse_impl::decimal<T>(str + dot_index + 1, str + e_index)) /
                  sgl::parse_impl::pow10<T>(static_cast<int>(e_index - dot_index - 1));
      auto exp = sgl::parse_impl::decimal<T>(str + e_index + 1, str + n);
      return (dec + frac) * sgl::parse_impl::pow10<T>(static_cast<int>(exp));
    }

  } // namespace parse_impl

  template <typename CharT, typename T>
  constexpr sgl::format_result to_chars(CharT* str, size_t len, T value) noexcept {
    return sgl::format_impl::basic_integer_format(str, len, value);
  }

  template <typename CharT>
  sgl::format_result
      to_chars(CharT* str, size_t len, float value, uint32_t precision, sgl::format fmt) noexcept {
    sgl::static_string<CharT, 24> buf{24, '\0'};

    unsigned size{0};
    switch (fmt) {
      case sgl::format::floating:
        size = ryu::f2s_buffered_n(value, buf.data());
        break;
      case sgl::format::exponential:
        size = ryu::d2exp_buffered_n(value, precision, buf.data());
        break;
      case sgl::format::fixed:
        size = ryu::d2fixed_buffered_n(value, precision, buf.data());
        break;
      case sgl::format::integer:
        size = ryu::d2fixed_buffered_n(gcem::round(value), 0, buf.data());
        break;
      case sgl::format::hex:
        return sgl::format_impl::basic_hex_format(str, len, ryu::to_bits(value));
        break;
      default:
        return {sgl::error::invalid_format, 0};
    }
    if (size > len) {
      return {sgl::error::buffer_too_small, 0};
    }
    buf.resize(size);
    for (const auto& ch : buf) {
      *str = ch;
      ++str;
    }
    return {sgl::error::no_error, static_cast<size_t>(size)};
  }

  template <typename CharT>
  sgl::format_result
      to_chars(CharT* str, size_t len, double value, uint32_t precision, sgl::format fmt) noexcept {
    sgl::static_string<CharT, 24> buf{24, '\0'};

    unsigned size{0};
    switch (fmt) {
      case sgl::format::floating:
        size = ryu::d2s_buffered_n(value, buf.data());
        break;
      case sgl::format::exponential:
        size = ryu::d2exp_buffered_n(value, precision, buf.data());
        break;
      case sgl::format::fixed:
        size = ryu::d2fixed_buffered_n(value, precision, buf.data());
        break;
      case sgl::format::integer:
        size = ryu::d2fixed_buffered_n(gcem::round(value), 0, buf.data());
        break;
      case sgl::format::hex:
        return sgl::format_impl::basic_hex_format(str, len, ryu::to_bits(value));
        break;
      default:
        return {sgl::error::invalid_format, 0};
    }
    if (size > len) {
      return {sgl::error::buffer_too_small, 0};
    }
    buf.resize(size);
    for (const auto& ch : buf) {
      *str = ch;
      ++str;
    }
    return {sgl::error::no_error, static_cast<size_t>(size)};
  }

  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                    str,
                                        size_t                    len,
                                        sgl::unsigned_fixed<I, F> value,
                                        uint32_t                  precision,
                                        sgl::format               fmt) noexcept {
    switch (fmt) {
      case sgl::format::integer:
        if (value.fraction() >= (gcem::pow(2ull, F) / 2))
          return format_impl::basic_integer_format(str, len, value.integer() + 1);
        return format_impl::basic_integer_format(str, len, value.integer());
        break;
      case sgl::format::fixed:
        break;
      case sgl::format::floating:
        [[fallthrough]];
      case sgl::format::exponential:
        if constexpr ((I + F) > 32) {
          return sgl::to_chars(str, len, sgl::to_double(value), precision, fmt);
        } else {
          return sgl::to_chars(str, len, sgl::to_float(value), precision, fmt);
        }
        break;
      case sgl::format::hex:
        return format_impl::basic_hex_format(str, len, value.value());
        break;
      default:
        return {sgl::error::invalid_format, 0};
    }
    if (len < 3)
      return {sgl::error::buffer_too_small, 0};

    sgl::static_string<CharT, 50> buf{len, '\0'};

    // frac is the raw integer value of the fractional part of value.
    // This needs to be converted to a "normal" integer value for formatting.
    // the formatting precision is equal to F.
    // The integer value needed can be calculated as follows:
    // value = round(frac / max_frac * 10^precision) = round(frac / 2^F * 10^precision) = round(frac
    // * 2^-F * 10^precision). factor = 2^-F * 10^precision can be calculated at compile time, with
    // only integer divisions at run time. This will unfortunately produce some rounding errors.
    constexpr auto factor =
        static_cast<uint64_t>(gcem::round(gcem::pow(10.0, F)) / gcem::pow(2.0, F));
    // fix point value to format after the decimal point
    auto res = sgl::format_impl::basic_integer_format(buf.data(), len, value.integer());
    if (res.ec != sgl::error::no_error) {
      return res;
    }
    if (len < (res.size + 1 + precision)) {
      return {sgl::error::buffer_too_small, 0};
    }

    if (precision < F) {
      auto frac_res = sgl::format_impl::basic_integer_format(buf.data() + res.size + 1,
                                                             len - res.size - 1,
                                                             (value.fraction() * factor) /
                                                                 gcem::pow(10u, F - precision));
      if (frac_res.ec != sgl::error::no_error)
        return frac_res;

      buf[res.size] = static_cast<CharT>('.');
      buf.resize(res.size + 1 + frac_res.size);
      for (const auto& ch : buf) {
        *str = ch;
        ++str;
      }
      return {sgl::error::no_error, res.size + 1 + frac_res.size};
    }

    auto frac_res = sgl::format_impl::basic_integer_format(buf.data() + res.size + 1,
                                                           len - res.size - 1,
                                                           (value.fraction() * factor) *
                                                               gcem::pow(10u, precision - F));
    if (frac_res.ec != sgl::error::no_error)
      return frac_res;

    buf[res.size] = static_cast<CharT>('.');
    buf.resize(res.size + 1 + frac_res.size);
    for (const auto& ch : buf) {
      *str = ch;
      ++str;
    }

    return {sgl::error::no_error, res.size + 1 + frac_res.size};
  }

  template <typename CharT, size_t I, size_t F>
  constexpr sgl::format_result to_chars(CharT*                  str,
                                        size_t                  len,
                                        sgl::signed_fixed<I, F> value,
                                        uint32_t                precision,
                                        sgl::format             fmt) noexcept {
    bool negative = value.is_negative();
    if (negative) {
      value = -value;
    }
    sgl::unsigned_fixed<I, F> u_val{value.value()};
    auto res = sgl::to_chars(str + negative, len - negative, u_val, precision, fmt);
    if (res.ec != sgl::error::no_error) {
      return res;
    }
    if (negative) {
      str[0] = static_cast<CharT>('-');
    }
    return {sgl::error::no_error, res.size + negative};
  }

  namespace cx {

    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          float       value,
                                          uint32_t    precision,
                                          sgl::format fmt) noexcept {
      sgl::static_string<CharT, 24> buf{};
      unsigned                      size{0};

      switch (fmt) {
        case sgl::format::floating:
          size = ryu::cx::f2s_buffered_n(value, buf.data());
          break;
        case sgl::format::exponential:
          size = ryu::cx::d2exp_buffered_n(value, precision, buf.data());
          break;
        case sgl::format::fixed:
          size = ryu::cx::d2fixed_buffered_n(value, precision, buf.data());
          break;
        case sgl::format::integer:
          size = ryu::cx::d2fixed_buffered_n(gcem::round(value), 0, buf.data());
          break;
        case sgl::format::hex:
          return sgl::format_impl::basic_hex_format(str, len, ryu::cx::to_bits(value));
          break;
        default:
          return {sgl::error::invalid_format, 0};
      }

      if (size > len) {
        return {sgl::error::format_error, 0};
      }

      buf.resize(size);
      for (const auto& ch : buf) {
        *str = ch;
        ++str;
      }

      return {sgl::error::no_error, static_cast<size_t>(size)};
    }

    template <typename CharT>
    constexpr sgl::format_result to_chars(CharT*      str,
                                          size_t      len,
                                          double      value,
                                          uint32_t    precision,
                                          sgl::format fmt) noexcept {
      sgl::static_string<CharT, 24> buf;

      unsigned size{0};
      switch (fmt) {
        case sgl::format::floating:
          size = ryu::cx::d2s_buffered_n(value, buf.data());
          break;
        case sgl::format::exponential:
          size = ryu::cx::d2exp_buffered_n(value, precision, buf.data());
          break;
        case sgl::format::fixed:
          size = ryu::cx::d2fixed_buffered_n(value, precision, buf.data());
          break;
        case sgl::format::integer:
          size = ryu::cx::d2fixed_buffered_n(gcem::round(value), 0, buf.data());
          break;
        case sgl::format::hex:
          return sgl::format_impl::basic_hex_format(str, len, ryu::cx::to_bits(value));
          break;
        default:
          return {sgl::error::invalid_format, 0};
      }

      if (size > len) {
        return {sgl::error::format_error, 0};
      }

      buf.resize(size);
      for (const auto& ch : buf) {
        *str = ch;
        ++str;
      }

      return {sgl::error::no_error, static_cast<size_t>(size)};
    }
  } // namespace cx

} // namespace sgl
#endif /* SGL_IMPL_FORMAT_IMPL_HPP */
