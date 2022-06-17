#ifndef SGL_FLOAT_ARG
#define SGL_FLOAT_ARG
#include "sgl_static_string.hpp"

#include <cstdint>

namespace sgl {
  /**
   * \brief This struct is used for compile time formatting of floating point values, used in
   * conjunction with the _double and _float operators defined in the sgl::cx_arg_literals
   * namespace.
   * \tparam T value type, i.e. float or double
   * \tparam N amount of characters in the literal
   */
  template <typename T, size_t N>
  struct cx_arg {
    T                      value{};  ///< parsed value
    static_string<char, N> string{}; ///< string containing characters plus null terminator
    /// unary negation operator so that expressions like ```-3.25_float are valid constructs```
    constexpr cx_arg<T, N + 1> operator-() {
      cx_arg<T, N + 1> ret;
      ret.value = -value;
      ret.string[0] = '-';
      for (size_t i = 0; i < N; ++i) {
        ret.string[i + 1] = string[i];
      }
      return ret;
    }
    /// unary plus operator for same reason as negation operator, just that this time the value does
    /// not change.
    constexpr cx_arg<T, N> operator+() { return *this; }
  };

  /// \cond
  namespace impl {
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

    constexpr auto decimal(const char* begin, const char* end) -> int64_t {
      if (begin == end)
        return 0;
      if (begin[0] == '-')
        return -decimal(begin + 1, end);
      if (begin[0] == '+')
        return decimal(begin + 1, end);
      int64_t         result{0};
      const long long size{end - begin};

      for (long long i = 0; i < size; ++i) {
        result += to_num(begin[i]) * sgl::impl::pow10<int64_t>(size - i - 1);
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


    constexpr double convert(const char* str, size_t n) {
      size_t dot_index{get_dot_index(str, n)};
      size_t e_index{get_e_index(str, n)};
      if (e_index == not_found) {
        // no exponent -> DEC[.]FRAC
        if (dot_index == not_found) {
          // DEC
          return static_cast<double>(decimal(str, str + n));
        } else {
          // [DEC].FRAC
          auto dec = decimal(str, str + dot_index);
          auto frac = static_cast<double>(decimal(str + dot_index + 1, str + n)) /
                      sgl::impl::pow10<double>(n - dot_index - 1);
          return static_cast<double>(dec) + frac;
        }
      } else {
        // has exponent
        // DEC[.]FRAC[Ee][+-]EXP
        if (dot_index == not_found) {
          // DEC[Ee][+-]EXP
          auto dec = decimal(str, str + e_index);
          auto exp = decimal(str + e_index + 1, str + n);
          return static_cast<double>(dec) * sgl::impl::pow10<double>(exp);
        } else {
          // [DEC].FRAC[Ee][+-]EXP
          auto dec = decimal(str, str + dot_index);
          auto frac = static_cast<double>(decimal(str + dot_index + 1, str + e_index)) /
                      sgl::impl::pow10<double>(e_index - dot_index - 1);
          auto exp = decimal(str + e_index + 1, str + n);
          return (dec + frac) * sgl::impl::pow10<double>(exp);
        }
      }
    }

  } // namespace impl
  /// \endcond

  namespace cx_arg_literals {

    /**
     * \brief create cx_arg double literal.
     * \note This operator merely parses a double out of of the chars given and returns a cx_arg
     * containing its value and a string of it **as it was declared**. Essentially, you, the
     * programmer, are the compile time formatter. See the example for more info.
     * \code
     * #include "sgl_cx_arg.hpp"
     * using namespace sgl::cx_arg_literals;
     * constexpr auto arg = 65.23_double; // arg.value = 65.23, arg.string = "65.23"
     * constexpr auto arg2 = .1_double; // arg.value = 0.1, arg.string = ".1"
     * constexpr auto arg3 = .100000_double; // arg.value = 0.1, arg.string = ".100000"
     * \endcode
     *
     * See https://en.cppreference.com/w/cpp/language/user_literal for more info.
     *
     * \tparam chars literal value
     */
    template <char... chars>
    constexpr cx_arg<double, sizeof...(chars)> operator"" _double() {
      char                             arr[sizeof...(chars)+1]{chars...};
      cx_arg<double, sizeof...(chars)> ret{0.0, arr};
      ret.value = sgl::impl::convert(ret.string.data(), sizeof...(chars));
      return ret;
    }
    /**
     * \brief create cx_arg float literal.
     * \details See _double udl operator.
     *
     * \code
     * #include "sgl_cx_arg.hpp"
     * constexpr auto arg = 65.23_float; // arg.value = 65.23f, arg.string = "65.23"
     * \endcode
     *
     * See https://en.cppreference.com/w/cpp/language/user_literal for more info.
     *
     * \tparam chars literal value
     */
    template <char... chars>
    constexpr cx_arg<float, sizeof...(chars)> operator"" _float() {
      char                             arr[sizeof...(chars)+1]{chars...};
      cx_arg<float, sizeof...(chars)> ret{0.0f, arr};
      ret.value = static_cast<float>(sgl::impl::convert(ret.string.data(), sizeof...(chars)));
      return ret;
    }
  } // namespace cx_arg_literals
} // namespace sgl
#endif