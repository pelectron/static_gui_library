#ifndef SGL_FLOAT_ARG
#define SGL_FLOAT_ARG
namespace sgl {
  template <typename T, size_t N>
  struct cx_arg {
    T    value{};
    char string[N + 1]{};
  };
  namespace impl {
    static constexpr size_t not_found{18446744073709551615ULL};

    template <char c, char... chars>
    constexpr size_t find_dot(size_t i = 0) {
      if constexpr (c == '.') {
        return i;
      } else if constexpr (sizeof...(chars) == 0)
        return not_found;
      else
        return find_dot<chars...>(i + 1);
    }
    template <char c, char... chars>
    constexpr size_t num_dots(size_t i = 0) {
      if constexpr (sizeof...(chars) == 0) {
        return c == '.' ? i + 1 : i;
      } else {
        return c == '.' ? num_dots<chars...>(i + 1) : num_dots<chars...>(i);
      }
    }

    template <char c, char... chars>
    constexpr size_t find_e(size_t i = 0) {
      if constexpr ((c == 'e') or (c == 'E')) {
        return i;
      } else if constexpr (sizeof...(chars) == 0)
        return not_found;
      else
        return find_e<chars...>(i + 1);
    }
    template <char c, char... chars>
    constexpr size_t num_es(size_t i = 0) {
      if constexpr (sizeof...(chars) == 0) {
        return ((c == 'e') || (c == 'E')) ? i + 1 : i;
      } else {
        return ((c == 'e') || (c == 'E')) ? num_es<chars...>(i + 1) : num_es<chars...>(i);
      }
    }
    constexpr size_t pow10(size_t n) {
      size_t i{1};
      while (n--) {
        i *= 10;
      }
      return i;
    }
    constexpr int pow10(int n) {
      int i{1};
      while (n--) {
        i *= 10;
      }
      return i;
    }
    static_assert(pow10(2) == 100, "");

    template <char c>
    constexpr int sign() {
      return c == '-' ? -1 : 1;
    }
    template <char c>
    constexpr int num() {
      return c - '0';
    }
    template <char c>
    constexpr bool is_num() {
      return (c >= '0') and (c <= '9');
    }

    template <size_t N, char c>
    constexpr int to_signed_first_n_first() {
      static_assert(N == 1, "N too big");
      static_assert(c != '-', "");
      static_assert(c != '+', "");
      return num<c>();
    }

    template <size_t N, char c, char... chars>
    constexpr int to_signed_first_n_rest(int previous_result) {
      if constexpr (N == 0)
        return previous_result;
      else if constexpr (N == 1)
        return previous_result + (previous_result > 0 ? num<c>() : -num<c>());
      else if constexpr (sizeof...(chars) != 0) {
        constexpr int digit = pow10(N - 1) * num<c>();
        int           result = previous_result + (previous_result > 0 ? digit : -digit);
        return to_signed_first_n_rest<N - 1, chars...>(result);
      } else {
        static_assert(sizeof...(chars) != 0, "N too big");
      }
    }

    template <size_t N, char c1, char c2, char... chars>
    constexpr int to_signed_first_n_first() {
      if constexpr (N == 0)
        return 0;
      if constexpr (N == 1)
        return is_num<c1>() ? num<c1>() : 0;
      if constexpr (is_num<c1>()) {
        constexpr int first_result = pow10(N - 1) * num<c1>();
        return to_signed_first_n_rest<N - 1, c2, chars...>(first_result);
      } else if constexpr (c1 == '-') {
        constexpr int first_result = -pow10(N - 1) * num<c2>();
        return to_signed_first_n_rest<N - 1, chars...>(first_result);
      } else if constexpr (c1 == '+') {
        constexpr int first_result = pow10(N - 1) * num<c2>();
        return to_signed_first_n_rest<N - 1, chars...>(first_result);
      } else {
        return 0;
      }
    };

    template <size_t N, char... chars>
    constexpr int to_signed_first_n() {
      return to_signed_first_n_first<N, chars...>();
    }
    template <size_t N, char c, char... chars>
    constexpr int to_signed_last_n_impl() {
      if constexpr (sizeof...(chars) != (N - 1)) {
        return to_signed_last_n_impl<N, chars...>();
      } else {
        return to_signed_first_n<N, c, chars...>();
      }
    }
    template <size_t N, char... chars>
    constexpr int to_signed_last_n() {
      return to_signed_last_n_impl<N, chars...>();
    }
    template <size_t Consumed, size_t dot_index, size_t e_index, char c, char... chars>
    constexpr int fractional_consume_front() {
      static_assert(dot_index != e_index, "");
      if constexpr (Consumed == (dot_index + 1)) {
        // '.' is now removed -> c, chars... == YYYe[+-]ZZZ
        constexpr size_t size = e_index - dot_index - 1;
        return to_signed_first_n<size, c, chars...>();
      } else {
        return fractional_consume_front<Consumed + 1, dot_index, e_index, chars...>();
      }
    }

    // parses YYY as an integer from [+-]XXX.YYYe[+-]ZZZ
    template <size_t dot_index, size_t e_index, char... chars>
    constexpr int fractional() {
      static_assert(dot_index <= e_index, "");
      // consume up until dot, then
      return fractional_consume_front<0, dot_index, e_index, chars...>();
    }
    template <typename T>
    constexpr T pow10(int exp) {
      T f{1.0};
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
    template <size_t N, size_t I, char c, char... chars>
    constexpr void fill_array_impl(char (&arr)[N + 1]) {
      arr[I] = c;
      if constexpr (sizeof...(chars) != 0) {
        fill_array_impl<N, I + 1, chars...>(arr);
      }
    }
    template <char... chars>
    constexpr void fill_array(char (&arr)[sizeof...(chars) + 1]) {
      fill_array_impl<sizeof...(chars), 0, chars...>(arr);
    }
    template <typename T, char... chars>
    constexpr cx_arg<T, sizeof...(chars)> make_cx_arg() {
      static_assert(num_dots<chars...>() <= 1, "invalid T literal. Found multiple '.' characters.");
      static_assert(num_es<chars...>() <= 1,
                    "invalid T literal. Found multiple 'e' or 'E' characters.");
      T                res{0.0f};
      constexpr size_t dot_index = find_dot<chars...>();
      constexpr size_t e_index = find_e<chars...>();
      static_assert(e_index, "");
      if constexpr (num_dots<chars...>() == 0) {
        // [+-]XXXe[+-]YYY or [+-]XXXe[+-]YYY
        static_assert(num_es<chars...>() == 1,
                      "floating point literals without a decimal point must contain an exponent.");
        constexpr int decimal = to_signed_first_n<e_index, chars...>();
        // size - index - 1 == number of characters left
        constexpr int exponent = to_signed_last_n<sizeof...(chars) - e_index - 1, chars...>();
        return static_cast<T>(decimal) * pow10<T>(exponent);
      } else {
        if constexpr (num_es<chars...>() == 0) {
          // [+-]XXX.YYY
          static_assert(num_dots<chars...>() != 0, "");
          constexpr int decimal = to_signed_first_n<dot_index, chars...>();
          constexpr int num_frac_digits = sizeof...(chars) - dot_index - 1;
          constexpr int fractional = to_signed_last_n<num_frac_digits, chars...>();
          res = static_cast<T>(decimal) + static_cast<T>(fractional) / pow10<T>(num_frac_digits);
        } else {
          // [+-]XXX.YYYe[+-]ZZZ
          static_assert(num_dots<chars...>() != 0, "");
          constexpr int    decimal = to_signed_first_n<dot_index, chars...>();
          constexpr int    exponent = to_signed_last_n<sizeof...(chars) - e_index - 1, chars...>();
          constexpr size_t num_frac_digits = e_index - dot_index - 1;
          constexpr int    fractional = ::sgl::impl::fractional<dot_index, e_index, chars...>();
          res = (static_cast<T>(decimal) + static_cast<T>(fractional) / pow10<T>(num_frac_digits)) *
                pow10<T>(exponent);
        }
      }
      cx_arg<T, sizeof...(chars)> ret;
      ret.value = res;
      fill_array<chars...>(ret.string);
      return ret;
    }
  } // namespace impl

  /// @endcond
  namespace cx_arg_literals {

    /**
     * @brief create cx_arg float literal.
     * @details An example fo how to use:
     *
     * @code
     * #include "sgl_cx_arg.hpp"
     * constexpr auto arg = 65.23_float; // arg.value = 65.23f, arg.string = "65.23"
     * @endcode
     *
     * See https://en.cppreference.com/w/cpp/language/user_literal for more info.
     *
     * @tparam chars literal value
     */
    template <char... chars>
    constexpr cx_arg<float, sizeof...(chars)> operator"" _float() {
      return impl::make_cx_arg<float, chars...>();
    }

    /**
     * @brief create cx_arg double literal.
     * @details An example fo how to use:
     *
     * @code
     * #include "sgl_cx_arg.hpp"
     * constexpr auto arg = 65.23_double; // arg.value = 65.23, arg.string = "65.23"
     * @endcode
     *
     * See https://en.cppreference.com/w/cpp/language/user_literal for more info.
     *
     * @tparam chars literal value
     */
    template <char... chars>
    constexpr cx_arg<double, sizeof...(chars)> operator"" _double() {
      return impl::make_cx_arg<double, chars...>();
    }
  } // namespace cx_arg_literals
} // namespace sgl
#endif