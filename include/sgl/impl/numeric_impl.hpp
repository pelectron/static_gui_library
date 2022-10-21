//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_NUMERIC_IMPL_HPP
#define SGL_IMPL_NUMERIC_IMPL_HPP
#include "sgl/numeric.hpp"

namespace sgl {
  /// deduction guides for Numeric
  /// \{

  template <size_t TextSize, typename CharT, typename T>
  Numeric(T initial_value, T delta, const CharT (&initial_text)[TextSize])
      -> Numeric<TextSize, CharT, std::decay_t<T>>;

  template <typename T, size_t CxSize>
  Numeric(const cx_arg<T, CxSize>& initial_value, T delta)
      -> Numeric<CxSize, char, std::decay_t<T>>;

  template <
      typename T,
      typename Formatter,
      enable_if_is_value_formatter<
          Formatter,
          Numeric<format_impl::max_buf_size_v<std::decay_t<T>>, char, std::decay_t<T>>> = true>
  Numeric(T initial_value, T delta, Formatter&& formatter)
      -> Numeric<format_impl::max_buf_size_v<std::decay_t<T>> + 2, char, std::decay_t<T>>;

  template <
      typename T,
      typename Formatter,
      typename InputHandler,
      enable_if_is_value_formatter<
          Formatter,
          Numeric<format_impl::max_buf_size_v<std::decay_t<T>>, char, std::decay_t<T>>> = true,
      enable_if_is_input_handler<
          InputHandler,
          Numeric<format_impl::max_buf_size_v<std::decay_t<T>>, char, std::decay_t<T>>> = true>
  Numeric(T initial_value, T delta, Formatter&&, InputHandler&&)
      -> Numeric<format_impl::max_buf_size_v<std::decay_t<T>> + 2, char, std::decay_t<T>>;

  /// \}

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>::Numeric(T initial_value, T delta) noexcept
      : Base(sgl::string_view<CharT>{}, &default_handle_input), format_(&default_format),
        value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>::Numeric(T          initial_value,
                                                 T          delta,
                                                 StringView initial_text) noexcept
      : Base(initial_text, &default_handle_input), value_(initial_value), delta_(delta) {}

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>::Numeric(const cx_arg<T, TextSize>& initial_value,
                                                 T                          delta) noexcept
      : Numeric<TextSize, CharT, T>(initial_value.value, delta, StringView(initial_value.string)) {}

  template <size_t TextSize, typename CharT, typename T>
  template <typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T>::Numeric(T           initial_value,
                                                 T           delta,
                                                 Formatter&& formatter) noexcept
      : Base(sgl::string_view<CharT>{}, &default_handle_input),
        format_(std::forward<Formatter>(formatter)), value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  template <typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
            enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T>::Numeric(T              initial_value,
                                                 T              delta,
                                                 Formatter&&    formatter,
                                                 InputHandler&& handler) noexcept
      : Base(sgl::string_view<CharT>{}, std::forward<InputHandler>(handler)),
        format_(std::forward<Formatter>(formatter)), value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr sgl::error Numeric<TextSize, CharT, T>::format(T val) noexcept {
    const size_t old_size = this->text().size();
    this->text().resize(TextSize);
    format_result res = format_(this->text().data(), TextSize, val, precision_, format_type_);
    if (res.ec != sgl::error::no_error) {
      this->text().resize(old_size);
      return res.ec;
    }
    this->text().resize(res.size);
    return res.ec;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr T Numeric<TextSize, CharT, T>::get_delta() const noexcept {
    return delta_;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr sgl::error Numeric<TextSize, CharT, T>::set_delta(T delta) noexcept {
    delta_ = delta;
    return sgl::error::no_error;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr T Numeric<TextSize, CharT, T>::get_value() const noexcept {
    return value_;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr sgl::error Numeric<TextSize, CharT, T>::set_value(T value) noexcept {
    sgl::error ec = this->format(value);
    if (ec == sgl::error::no_error) {
      value_ = value;
    }
    return ec;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr void Numeric<TextSize, CharT, T>::set_precision(uint32_t precision) noexcept {
    precision_ = precision;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr void Numeric<TextSize, CharT, T>::set_format(sgl::format format) noexcept {
    format_ = format;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr sgl::error
      Numeric<TextSize, CharT, T>::default_handle_input(Numeric<TextSize, CharT, T>& numeric_item,
                                                        sgl::Input input) noexcept {
    if (!is_keyboard_input(input)) {
      switch (input) {
        case sgl::Input::up:
          [[fallthrough]];
        case sgl::Input::right:
          numeric_item.set_value(numeric_item.get_value() + numeric_item.get_delta());
          break;
        case sgl::Input::down:
          [[fallthrough]];
        case sgl::Input::left:
          numeric_item.set_value(numeric_item.get_value() - numeric_item.get_delta());
          break;
        default:
          break;
      }
    }
    return sgl::error::no_error;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr sgl::format_result
      Numeric<TextSize, CharT, T>::default_format(CharT*      str,
                                                  size_t      len,
                                                  T           value,
                                                  uint32_t    precision,
                                                  sgl::format format) noexcept {
    if constexpr (std::is_integral_v<T>)
      return sgl::to_chars(str, len, value);
    else
      return sgl::to_chars(str, len, value, precision, format);
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T> make_numeric(T initial_value, T delta) noexcept {
    return Numeric<TextSize, CharT, T>(initial_value, delta);
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>
      make_numeric(T initial_value, T delta, sgl::string_view<CharT> initial_text) noexcept {
    return Numeric<TextSize, CharT, T>(initial_value, delta, initial_text);
  }

  template <size_t CxSize, typename T, size_t TextSize>
  constexpr Numeric<TextSize, char, T> make_numeric(const cx_arg<T, CxSize>& initial_value,
                                                    T                        delta) noexcept {
    return Numeric<TextSize, char, T>(initial_value, delta);
  }

  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T>
      make_numeric(T initial_value, T delta, Formatter&& formatter) noexcept {
    return Numeric<TextSize, CharT, T>(initial_value, delta, std::forward<Formatter>(formatter));
  }

  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
            enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T> make_numeric(T              initial_value,
                                                     T              delta,
                                                     Formatter&&    formatter,
                                                     InputHandler&& handler) noexcept {
    return Numeric<TextSize, CharT, T>(initial_value,
                                       delta,
                                       std::forward<Formatter>(formatter),
                                       std::forward<InputHandler>(handler));
  }
} // namespace sgl
#endif /* SGL_IMPL_NUMERIC_IMPL_HPP */
