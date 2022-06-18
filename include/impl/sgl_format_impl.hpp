#ifndef SGL_FORMAT_IMPL_HPP
#define SGL_FORMAT_IMPL_HPP
#include "../sgl_config.h"
#include "../sgl_limits.hpp"
#include "../sgl_static_string.hpp"

#include <type_traits>

#if SGL_USE_RYU
  #include "ryu/ryu.h"
  #include "ryu/ryu_parse.h"
#endif
#if SGL_USE_CHARCONV
  #include <charconv>
#endif

#include "../sgl_format.hpp"

namespace sgl {
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
    static constexpr size_t value = 1000000000;
  };
  template <>
  struct biggest_pow10_t<int64_t> {
    static constexpr size_t value = 1000000000;
  };

  template <typename T>
  static constexpr size_t biggest_pow10_v = biggest_pow10_t<T>::value;

  template <typename T>
  constexpr size_t biggest_pow10(T val) {
    if ((val >= T{1000000000ull}) and (numeric_limits<T>::max() > 1000000000ull))
      return 1000000000;
    else if ((val >= T{100000000ull}) and (numeric_limits<T>::max() > 100000000ull))
      return 100000000;
    else if ((val >= T{10000000ull}) and (numeric_limits<T>::max() > 10000000ull))
      return 10000000;
    else if ((val >= T{1000000ull}) and (numeric_limits<T>::max() > 1000000ull))
      return 1000000;
    else if ((val >= T{100000ull}) and (numeric_limits<T>::max() > 100000ull))
      return 100000;
    else if ((val >= T{10000ull}) and (numeric_limits<T>::max() > 10000ull))
      return 10000;
    else if ((val >= T{1000ull}) and (numeric_limits<T>::max() > 1000ull))
      return 1000;
    else if (val >= T{100ull})
      return 100;
    else if (val >= T{10ull})
      return 10;
    else
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
    static constexpr size_t value = 7;
  };
  template <>
  struct max_buf_size<int32_t> {
    static constexpr size_t value = 8;
  };
  template <>
  struct max_buf_size<uint64_t> {
    static constexpr size_t value = 10;
  };
  template <>
  struct max_buf_size<int64_t> {
    static constexpr size_t value = 11;
  };
  template <>
  struct max_buf_size<float> {
    static constexpr size_t value = 15;
  };
  template <>
  struct max_buf_size<double> {
    static constexpr size_t value = 24;
  };
  template <typename T>
  constexpr auto biggest_pow16(T value) {
    if ((numeric_limits<T>::max() >= 0x1000000000000000) and (value > T{0x1000000000000000}))
      return 0x1000000000000000;
    else if ((numeric_limits<T>::max() >= 0x100000000000000) and (value > T{0x100000000000000}))
      return 0x100000000000000;
    else if ((numeric_limits<T>::max() >= 0x10000000000000) and (value > T{0x10000000000000}))
      return 0x10000000000000;
    else if ((numeric_limits<T>::max() >= 0x1000000000000) and (value > T{0x1000000000000}))
      return 0x1000000000000;
    else if ((numeric_limits<T>::max() >= 0x100000000000) and (value > T{0x100000000000}))
      return 0x100000000000;
    else if ((numeric_limits<T>::max() >= 0x10000000000) and (value > T{0x10000000000}))
      return 0x10000000000;
    else if ((numeric_limits<T>::max() >= 0x1000000000) and (value > T{0x1000000000}))
      return 0x1000000000;
    else if ((numeric_limits<T>::max() >= 0x100000000) and (value > T{0x100000000}))
      return 0x100000000;
    else if ((numeric_limits<T>::max() >= 0x10000000) and (value > T{0x10000000}))
      return 0x10000000;
    else if ((numeric_limits<T>::max() >= 0x1000000) and (value > T{0x1000000}))
      return 0x1000000;
    else if ((numeric_limits<T>::max() >= 0x100000) and (value > T{0x100000}))
      return 0x100000;
    else if ((numeric_limits<T>::max() >= 0x10000) and (value > T{0x10000}))
      return 0x10000;
    else if ((numeric_limits<T>::max() >= 0x1000) and (value > T{0x1000}))
      return 0x1000;
    else if ((numeric_limits<T>::max() >= 0x100) and (value > T{0x100}))
      return 0x100;
    else if ((numeric_limits<T>::max() >= 0x10) and (value > T{0x10}))
      return 0x10;
    else
      return 0x1;
  }

  template <typename T>
  static constexpr size_t max_buf_size_v = max_buf_size<T>::value;

  template <typename CharT, typename T>
  constexpr sgl::error basic_hex_format(CharT* str, size_t len, T value) {
    constexpr auto hex_char = [](T val) -> CharT {
      if (val >= 0 && val <= 9) {
        return static_cast<CharT>(val + '0');
      }
      if (val >= 10 && val <= 15) {
        return static_cast<CharT>(val + 'A' - 10);
      }
      return 0;
    };
    static_assert(std::is_integral_v<T>, "");
    // static_assert(std::is_unsigned_v<T>, "");
    constexpr size_t           size = 2 * sizeof(T) + 2; // +2 for '0x'
    static_string<CharT, size> buf;
    buf.append("0x", 2);
    for (size_t pow16 = biggest_pow16(value); pow16 != 0; pow16 /= 16) {
      buf.append(hex_char(value / pow16));
      value = value % pow16;
    }
    for (const auto& ch : buf) {
      *str = ch;
      ++str;
    }
    *str = '\0';
    return sgl::error::no_error;
  }

  template <typename CharT, typename T>
  constexpr sgl::error basic_integer_format(CharT* str, size_t len, T value) {
    static_string<CharT, max_buf_size_v<T>> buf;
    static_assert(std::is_integral_v<T>, "");
    if constexpr (std::is_signed_v<T>) {
      if (value < 0) {
        buf.append(CharT{'-'});
        value = -value;
      }
    }
    for (size_t pow10 = biggest_pow10(value); pow10 != 0; pow10 /= 10) {
      buf.append(static_cast<CharT>(value / pow10 + '0'));
      value = value % pow10;
    }
    for (const auto& ch : buf) {
      *str = ch;
      ++str;
    }
    *str = '\0';
    return sgl::error::no_error;
  }

  template <typename CharT, typename T>
  constexpr sgl::error integer_format(CharT*   str,
                                      size_t   len,
                                      T        value,
                                      uint32_t precision,
                                      format_t format) noexcept {
    static_assert(std::is_integral_v<T>, "");
    (void)precision;
    switch (format) {
      case format_t::exponential:
        [[fallthrough]];
      case format_t::fixed:
      case format_t::floating:
      case format_t::integer:
        return basic_integer_format(str, len, value);
      case format_t::hex:
        if constexpr (std::is_signed_v<T>) {
          return sgl::error::invalid_format;
        } else {
          return basic_hex_format(str, len, value);
        }
      default:
        return sgl::error::invalid_format;
    }

    return sgl::error::null_format;
  }

#if SGL_USE_RYU
  template <typename CharT, typename T>
  sgl::error parse(const CharT* str, const size_t len, T& value) noexcept {
    if constexpr (std::is_same_v<double, T>) {
      static_assert(std::is_same_v<CharT, char>,
                    "only CharT=char supported with floating point parsing");

      if (s2d_n(str, len, &value) == Status::SUCCESS)
        return sgl::error::no_error;
      else
        return sgl::error::format_error;

    } else if constexpr (std::is_same_v<float, T>) {
      static_assert(std::is_same_v<CharT, char>,
                    "only CharT=char supported with floating point parsing");

      if (s2f_n(str, &value) == Status::SUCCESS)
        return sgl::error::no_error;
      else
        return sgl::error::format_error;

    } else {

      if ((len > max_buf_size_v<T>) || (len == 0))
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

  template <typename CharT, typename T>
  sgl::error format(CharT* str, size_t len, T value, uint32_t precision, format_t format) noexcept {
    if constexpr (std::is_floating_point_v<T>) {
      if constexpr (std::is_same_v<double, T>) {
        if (len < max_buf_size_v<T>)
          return sgl::error::format_error;

        sgl::static_string<char, max_buf_size_v<T>> buf;
        int                                         size{0};
        switch (format) {
          case format_t::floating:
            size = d2s_buffered_n(value, buf.data());
            break;
          case format_t::exponential:
            size = d2exp_buffered_n(value, precision, buf.data());
            break;
          case format_t::fixed:
            size = d2fixed_buffered_n(value, precision, buf.data());
            break;
          case format_t::integer:
            size = d2fixed_buffered_n(value, 0, buf.data());
            break;
          default:
            return sgl::error::invalid_format;
        }
        buf.resize(size);
        for (const auto& ch : buf) {
          *str = ch;
          ++str;
        }
        str[size] = '\0';
        return sgl::error::no_error;
      } else if constexpr (std::is_same_v<float, T>) {
        sgl::static_string<char, max_buf_size_v<T>> buf(max_buf_size_v<T>, '\0');

        int size{0};
        switch (format) {
          case format_t::floating:
            size = f2s_buffered_n(value, buf.data());
            break;
          case format_t::exponential:
            size = d2exp_buffered_n(value, precision, buf.data());
            break;
          case format_t::fixed:
            size = d2fixed_buffered_n(value, precision, buf.data());
            break;
          case format_t::integer:
            size = d2fixed_buffered_n(value, 0, buf.data());
            break;
          default:
            return sgl::error::invalid_format;
        }
        buf.resize(size);
        for (const auto& ch : buf) {
          *str = ch;
          ++str;
        }
        str[size] = '\0';
        return sgl::error::no_error;
      }
    } else {
      return integer_format(str, len, value, precision, format);
    }
  }

#endif

#if SGL_USE_CHARCONV
  template <typename CharT, typename T>
  sgl::error format(CharT* str, size_t len, T value, uint32_t precision, format_t format) noexcept {
    (void)precision;
    (void)format;
    auto res = std::to_chars(str, str + len, value);
    if (res.ec == std::errc()) {
      return sgl::error::no_error;
    } else {
      return sgl::error::format_error;
    }
  }

  template <typename CharT, typename T>
  sgl::error parse(const CharT* str, const size_t len, T& value) noexcept {
    (void)precision;
    (void)format;
    auto res = std::from_chars(str, str + len, value);
    if (res.ec == std::errc()) {
      return sgl::error::no_error;
    } else {
      return sgl::error::format_error;
    }
  }
#endif

#if !SGL_USE_CHARCONV && !SGL_USE_RYU
  template <typename CharT, typename T>
  constexpr sgl::error
      format(CharT* str, size_t len, T value, uint32_t precision, format_t format) noexcept {
    return integer_format(str, len, value, precision, format);
  }
#endif
  namespace parse_impl {
    template <typename T>
    constexpr T pow10(int exp) {
      T f{1};
      if (exp < 0) {
        exp = -exp;
        while (exp--) {
          f /= 10;
        }
      } else if (exp > 0) {
        while (exp--) {
          f *= 10;
        }
      }
      return f;
    }

    static constexpr size_t not_found{18446744073709551615ULL};

    constexpr auto to_num(char c) -> int64_t { return (c - '0'); };

    template <typename T>
    constexpr T decimal(const char* begin, const char* end) {
      if (begin == end)
        return 0;
      if (begin[0] == '-')
        return -decimal(begin + 1, end);
      if (begin[0] == '+')
        return decimal(begin + 1, end);

      T               result{0};
      const long long size{end - begin};

      for (long long i = 0; i < size; ++i) {
        result += to_num(begin[i]) * sgl::impl::pow10<T>(size - i - 1);
      }
      return result;
    };

    constexpr auto get_dot_index(const char* str, size_t n) {
      for (size_t i = 0; i < n; ++i) {
        if (str[i] == '.')
          return i;
      }
      return sgl::impl::not_found;
    };

    constexpr auto get_e_index(const char* str, size_t n) {
      for (size_t i = 0; i < n; ++i) {
        if ((str[i] == 'e') || (str[i] == 'E'))
          return i;
      }
      return sgl::impl::not_found;
    };

    template <typename T>
    constexpr T parse(const char* str, size_t n) {
      if constexpr (std::is_integral_v<T>) {
        return decimal<T>(str, str + n);
      } else {
        size_t dot_index{get_dot_index(str, n)};
        size_t e_index{get_e_index(str, n)};
        if (e_index == not_found) {
          // no exponent -> DEC[.]FRAC
          if (dot_index == not_found) {
            // DEC
            return static_cast<T>(decimal<T>(str, str + n));
          } else {
            // [DEC].FRAC
            auto dec = decimal<T>(str, str + dot_index);
            auto frac = static_cast<T>(decimal<T>(str + dot_index + 1, str + n)) /
                        sgl::impl::pow10<T>(n - dot_index - 1);
            return static_cast<T>(dec) + frac;
          }
        } else {
          // has exponent
          // DEC[.]FRAC[Ee][+-]EXP
          if (dot_index == not_found) {
            // DEC[Ee][+-]EXP
            auto dec = decimal<T>(str, str + e_index);
            auto exp = decimal<T>(str + e_index + 1, str + n);
            return static_cast<T>(dec) * sgl::impl::pow10<T>(exp);
          } else {
            // [DEC].FRAC[Ee][+-]EXP
            auto dec = decimal<T>(str, str + dot_index);
            auto frac = static_cast<T>(decimal<T>(str + dot_index + 1, str + e_index)) /
                        sgl::impl::pow10<T>(e_index - dot_index - 1);
            auto exp = decimal<T>(str + e_index + 1, str + n);
            return (dec + frac) * sgl::impl::pow10<T>(exp);
          }
        }
      }
    }

  } // namespace parse_impl
} // namespace sgl
#endif