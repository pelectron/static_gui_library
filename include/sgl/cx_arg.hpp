/// @file cx_arg.hpp
/// @author Pelé Constam(pelectron1602 @gmail.com)
/// @version 0.1
/// @date 2022 - 09 -24
///
/// @copyright
///              Copyright Pele Constam 2022.
/// Distributed under the Boost Software License, Version 1.0.
///    (See accompanying file LICENSE_1_0.txt or copy at
///          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SGL_CX_ARG_HPP
#define SGL_CX_ARG_HPP
#include "sgl/format.hpp"
#include "sgl/static_string.hpp"

namespace sgl {

  /**
    This struct is used for compile time formatting of floating point values, used in
    conjunction with the _double and _float operators defined in the sgl::cx_arg_literals
    namespace.
    @tparam T value type, i.e. float or double
    @tparam Size amount of characters in the literal
  */
  template <typename T, size_t Size>
  struct cx_arg {
    T                      value{};  /// < parsed value
    static_string<char, Size> string{}; ///< string containing characters plus null terminator

    /**
      unary negation
      operator so that expressions like ```- 3.25_float``` are valid constructs.
      @return cx_arg<T, Size + 1>
    */
    constexpr cx_arg<T, Size + 1> operator-() {
      cx_arg<T, Size + 1> ret;
      ret.value = -value;
      ret.string[0] = '-';
      for (size_t i = 0; i < Size; ++i) {
        ret.string[i + 1] = string[i];
      }
      return ret;
    }

    /**
      unary plus operator for same reason as negation operator,
      just that this time the value does not change.
      @return cx_arg<T, Size>
    */
    constexpr cx_arg<T, Size> operator+() { return *this; }
  };

  namespace cx_arg_literals {
    /**
      create cx_arg double literal.
      @note This operator merely parses a double out of of the chars given and returns a cx_arg
      containing its value and a string of it **as it was declared**. Essentially, you, the
      programmer, are the compile time formatter. See the example below for more info.

      ```cpp
      #include "sgl_cx_arg.hpp"
      using namespace sgl::cx_arg_literals;
      constexpr auto arg = 65.23_double; // arg.value = 65.23, arg.string = "65.23"
      constexpr auto arg2 = .1_double; // arg.value = 0.1, arg.string = ".1"
      constexpr auto arg3 = .100000_double; // arg.value = 0.1, arg.string = ".100000"
      ```

      See https://en.cppreference.com/w/cpp/language/user_literal for more info.
      @tparam chars literal value
    */
    template <char... chars>
    constexpr cx_arg<double, sizeof...(chars)> operator"" _double() {
      constexpr char                   arr[sizeof...(chars) + 1]{chars..., '\0'};
      cx_arg<double, sizeof...(chars)> ret{0.0, static_string{arr}};
      ret.value = sgl::parse_impl::parse<double>(ret.string.data(), sizeof...(chars));
      return ret;
    }

    /**
      create cx_arg float literal.
      See _double udl operator.

      ```cpp
      #include "sgl_cx_arg.hpp"
      constexpr auto arg = 65.23_float; // arg.value = 65.23f, arg.string = "65.23"
      ```

      See https://en.cppreference.com/w/cpp/language/user_literal for more info.
      @tparam chars literal value
    */
    template <char... chars>
    constexpr cx_arg<float, sizeof...(chars)> operator"" _float() {
      constexpr char                  arr[sizeof...(chars) + 1]{chars..., '\0'};
      cx_arg<float, sizeof...(chars)> ret{0.0f, static_string{arr}};
      ret.value =
          static_cast<float>(sgl::parse_impl::parse<float>(ret.string.data(), sizeof...(chars)));
      return ret;
    }
  } // namespace cx_arg_literals
} // namespace sgl
#endif /* SGL_CX_ARG_HPP */
