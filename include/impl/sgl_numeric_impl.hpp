#ifndef SGL_NUMERIC_IMPL_HPP
#define SGL_NUMERIC_IMPL_HPP
#include "../sgl_format.hpp"
#include "../sgl_numeric.hpp"

namespace sgl {
  /// deduction guides for Numeric
  /// \{
  template <typename CharT, typename T>
  Numeric(sgl::string_view<CharT> name, T initial_value, T delta)
      -> Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>;

  template <typename CharT, typename T, size_t N>
  Numeric(const CharT (&name)[N], T initial_value, T delta)
      -> Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>;

  template <size_t TextSize, typename CharT, typename T, size_t N>
  Numeric(const CharT (&name)[N], T initial_value, const CharT (&initial_text)[TextSize], T delta)
      -> Numeric<TextSize, CharT, decay_t<T>>;

  template <typename CharT, typename T, size_t CxSize>
  Numeric(sgl::string_view<CharT> name, const cx_arg<T, CxSize>& initial_value, T delta)
      -> Numeric<CxSize, CharT, decay_t<T>>;

  template <typename CharT, typename T, size_t CxSize, size_t N>
  Numeric(const CharT (&name)[N], const cx_arg<T, CxSize>& initial_value, T delta)
      -> Numeric<CxSize, CharT, decay_t<T>>;

  template <
      typename CharT,
      typename T,
      typename Formatter,
      enable_if_is_value_formatter<Formatter,
                                   Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>> = true>
  Numeric(sgl::string_view<CharT> name, T initial_value, T delta, Formatter&& formatter)
      -> Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>;

  template <
      typename CharT,
      typename T,
      typename Formatter,
      typename InputHandler,
      enable_if_is_value_formatter<Formatter,
                                   Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>> = true,
      enable_if_is_input_handler<InputHandler,
                                 Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>> = true>
  Numeric(sgl::string_view<CharT> name, T initial_value, T delta, Formatter&&, InputHandler&&)
      -> Numeric<max_buf_size_v<decay_t<T>>, CharT, decay_t<T>>;
  /// \}

  template <size_t TextSize, typename CharT, typename T>
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
  template <size_t CxSize>
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
  constexpr Numeric<TextSize, CharT, T>::Numeric(StringView  name,
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
  constexpr Numeric<TextSize, CharT, T>::Numeric(StringView     name,
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

  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     T                       delta) noexcept {
    return Numeric<TextSize, CharT, T>(name, initial_value, delta);
  }
  template <size_t TextSize, typename CharT, typename T, size_t N>
  constexpr Numeric<TextSize, CharT, T> make_numeric(const CharT (&name)[N],
                                                     T initial_value,
                                                     T delta) noexcept {
    return make_numeric<TextSize>(sgl::string_view<CharT>(name), initial_value, delta);
  }
  template <size_t TextSize, typename CharT, typename T>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     sgl::string_view<CharT> initial_text,
                                                     T                       delta) noexcept {
    return Numeric<TextSize, CharT, T>(name, initial_value, initial_text, delta);
  }

  template <size_t TextSize, typename T, size_t CxSize>
  constexpr Numeric<TextSize, char, T> make_numeric(sgl::string_view<char>   name,
                                                    const cx_arg<T, CxSize>& initial_value,
                                                    T                        delta) noexcept {
    return Numeric<TextSize, char, T>(name, initial_value, delta);
  }

  template <size_t TextSize, typename T, size_t N, size_t CxSize>
  constexpr Numeric<TextSize, char, T> make_numeric(const char (&name)[N],
                                                    const cx_arg<T, CxSize>& initial_value,
                                                    T                        delta) noexcept {
    return make_numeric<TextSize>(sgl::string_view<char>(name), initial_value, delta);
  }

  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     T                       delta,
                                                     Formatter&&             formatter) noexcept {
    return Numeric<TextSize, CharT, T>(name, initial_value, delta, forward<Formatter>(formatter));
  }

  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>,
            enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>>>
  constexpr Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
                                                     T                       initial_value,
                                                     T                       delta,
                                                     Formatter&&             formatter,
                                                     InputHandler&&          handler) noexcept {
    return Numeric<TextSize, CharT, T>(name,
                                       initial_value,
                                       delta,
                                       forward<Formatter>(formatter),
                                       forward<InputHandler>(handler));
  }
} // namespace sgl
#endif