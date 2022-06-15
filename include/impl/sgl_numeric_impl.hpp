#ifndef SGL_NUMERIC_IMPL_HPP
#define SGL_NUMERIC_IMPL_HPP
#include "../sgl_numeric.hpp"
#include "../sgl_format.hpp"

namespace sgl{
  template <size_t TextSize, typename CharT, typename T>
  template <typename T_, enable_if_t<is_floating_point_v<T_>, bool>>
  Numeric<TextSize, CharT, T>::Numeric(StringView name, T initial_value, T delta) noexcept
      : Base(name, &default_handle_input), format_(&default_format), value_(initial_value),
        delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  template <typename T_, enable_if_t<is_integral_v<T_>, bool>>
  constexpr Numeric<TextSize, CharT, T>::Numeric(StringView name, T initial_value, T delta) noexcept
      : Base(name, &default_handle_input), format_(&default_format), value_(initial_value),
        delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T>::Numeric(StringView name,
                                                 T          initial_value,
                                                 StringView initial_text,
                                                 T          delta) noexcept
      : Base(name, initial_text, &default_handle_input), value_(initial_value), delta_(delta) {
    this->set_text(initial_text);
  }

  template <size_t TextSize, typename CharT, typename T>
  template <size_t CxSize,
            typename T_,
            sgl::enable_if_t<sgl::is_same_v<CharT, char> and sgl::is_floating_point_v<T_>, bool>>
  constexpr Numeric<TextSize, CharT, T>::Numeric(StringView               name,
                                                 const cx_arg<T, CxSize>& initial_value,
                                                 T                        delta) noexcept
      : Numeric<TextSize, CharT, T>(name,
                                    initial_value.value,
                                    StringView(initial_value.string),
                                    delta) {}

  template <size_t TextSize, typename CharT, typename T>
  template <typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  Numeric<TextSize, CharT, T>::Numeric(StringView  name,
                                       T           initial_value,
                                       T           delta,
                                       Formatter&& formatter) noexcept
      : Base(name, &default_handle_input), format_(sgl::forward<Formatter>(formatter)),
        value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  template <typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
            enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>>>
  Numeric<TextSize, CharT, T>::Numeric(StringView     name,
                                       T              initial_value,
                                       T              delta,
                                       Formatter&&    formatter,
                                       InputHandler&& handler) noexcept
      : Base(name, sgl::forward<InputHandler>(handler)),
        format_(sgl::forward<Formatter>(formatter)), value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr sgl::error Numeric<TextSize, CharT, T>::format(static_string<CharT, TextSize>& str,
                                                           T val) noexcept {
    return format_(str, val);
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
    format_buffer_.resize(TextSize);
    sgl::error ec = format_(format_buffer_, value);
    if (ec == sgl::error::no_error) {
      value_ = value;
      this->set_text(format_buffer_);
    }
    format_buffer_.reset();
    return ec;
  }

  template <size_t TextSize, typename CharT, typename T>
  constexpr static_string<CharT, TextSize>& Numeric<TextSize, CharT, T>::get_buffer() noexcept {
    return format_buffer_;
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
  constexpr sgl::error
      Numeric<TextSize, CharT, T>::default_format(static_string<CharT, TextSize>& text,
                                                  T                               value) noexcept {
    if constexpr (sgl::is_integral_v<T>) {
      return integer_format(text.data(), text.size(), value, 6, sgl::format_t::fixed);
    } else
      return sgl::format(text.data(), text.size(), value, 6, sgl::format_t::fixed);
  }
}
#endif