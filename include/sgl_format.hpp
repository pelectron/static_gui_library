#ifndef SGL_FORMAT_HPP
#define SGL_FORMAT_HPP
#include "sgl_config.h"
#include "sgl_error.hpp"
#include "sgl_limits.hpp"
#include "sgl_static_string.hpp"
#include "sgl_type_traits.hpp"

#include <cstdint>


#if SGL_USE_RYU
  #include "ryu/ryu.h"
  #include "ryu/ryu_parse.h"
#endif
#if SGL_USE_CHARCONV
  #include <charconv>
#endif

namespace sgl {

  enum class format_t { integer, floating, exponential, fixed };
  /// @cond
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
  static constexpr size_t max_buf_size_v = max_buf_size<T>::value;

  template <typename CharT, typename T>
  constexpr sgl::error integer_format(CharT*   str,
                                      size_t   len,
                                      T        value,
                                      uint32_t precision,
                                      format_t format) noexcept {
    (void)precision;
    (void)format;
    (void)len;
    if constexpr (is_integral_v<T>) {
      static_string<CharT, max_buf_size_v<T>> buf;
      static_assert(is_integral_v<T>, "");
      if constexpr (is_signed_v<T>) {
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
    return sgl::error::null_format;
  }
/// @endcond
#if SGL_USE_RYU
  template <typename CharT, typename T>
  sgl::error parse(const CharT* str, const size_t len, T& value) noexcept {
    if constexpr (is_same_v<double, T>) {
      static_assert(is_same_v<CharT, char>,
                    "only CharT=char supported with floating point parsing");

      if (s2d_n(str, len, &value) == Status::SUCCESS)
        return sgl::error::no_error;
      else
        return sgl::error::format_error;

    } else if constexpr (is_same_v<float, T>) {
      static_assert(is_same_v<CharT, char>,
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
    if constexpr (is_floating_point_v<T>) {
      if constexpr (is_same_v<double, T>) {
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
      } else if constexpr (is_same_v<float, T>) {
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
      (void)precision;
      (void)format;
      static_string<CharT, max_buf_size_v<T>> buf;
      static_assert(is_integral_v<T>, "");
      if constexpr (is_signed_v<T>) {
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
} // namespace sgl
#endif