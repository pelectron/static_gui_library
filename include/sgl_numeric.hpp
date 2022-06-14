#ifndef SGL_BASIC_NUMERIC_HPP
#define SGL_BASIC_NUMERIC_HPP
#include "item_traits.hpp"
#include "sgl_cx_arg.hpp"
#include "sgl_format.hpp"
#include "sgl_item_base.hpp"

namespace sgl {
  /**
   * @brief This class models a numeric item. It consists of a value of type T,
   * a delta value of type T and a formatter to format T's into strings.
   * @details The default behaviour of this item when it is edited is as follows: an input of
   * sgl::Input::up/right increases the item's value by it's delta value, i.e. value += delta. An
   * input equal to sgl::Input::down/left decreases the item's value by it's delta value, i.e. value
   * -= value.
   * @tparam T value type
   * @tparam TextSize display width
   * @tparam CharT character type
   */
  template <size_t TextSize, typename CharT, typename T>
  class Numeric : public sgl::ItemBase<traits<Numeric<TextSize, CharT, T>>> {
  public:
    static_assert(is_integral_v<T> || is_floating_point_v<T>, "T must be an integral type");
    static_assert(!is_same_v<bool, T>, "T must not be bool. Use Boolean_t for a boolean item.");
    using item_type = Numeric<TextSize, CharT, T>;
    using Base = sgl::ItemBase<traits<item_type>>;

    using StringView = typename Base::StringView;
    using String = typename Base::String;
    using value_type = T;
    /// concrete formatter type
    using Formatter_t = Callable<sgl::error(static_string<CharT, TextSize>&, value_type)>;

    static constexpr size_t text_size = TextSize;

    /**
     * @brief Construct a new Numeric with default formatter and default
     * input handling. This constructor is constexpr if T is integral
     * @param name name of item
     * @param initial_value initial value
     * @param delta delta value
     * @{
     */
    template <typename T_ = T, enable_if_t<is_floating_point_v<T_>, bool> = true>
    Numeric(StringView name, T initial_value, T delta) noexcept;

    template <typename T_ = T, enable_if_t<is_integral_v<T_>, bool> = true>
    constexpr Numeric(StringView name, T initial_value, T delta) noexcept;
    /// @}

    constexpr Numeric(StringView name, T initial_value, StringView initial_text, T delta) noexcept;

    /**
     * @brief create a Numeric item in constexpr with a sgl::cx_arg.
     * @details If you don't need a constexpr Numeric, use the overload with an ordinary T as
     * initial value. This is only needed for floating point numeric items. A quick example:
     * @code
     *    constexpr auto num = Numeric<float, 40, char>("num item", 64.534_float,0.1f);
     * @endcode
     *
     * @tparam CxSize cx_arg string size.
     * @param name name of item
     * @param initial_value initial value as cx_arg
     * @param delta delta value
     */
    template <
        size_t CxSize,
        typename T_ = T,
        sgl::enable_if_t<sgl::is_same_v<CharT, char> and sgl::is_floating_point_v<T_>, bool> = true>
    constexpr Numeric(StringView name, const cx_arg<T, CxSize>& initial_value, T delta) noexcept;

    /**
     * @brief Construct a new Numeric with custom formatter, but default
     * input handling.
     *
     * @tparam Formatter formatter type, see formatter_check for more details.
     * @param name name of item
     * @param initial_value inital value
     * @param delta delta value
     * @param formatter formatter instance
     */
    template <typename Formatter, enable_if_is_value_formatter<Formatter, item_type> = true>
    Numeric(StringView name, T initial_value, T delta, Formatter&& formatter) noexcept;

    /**
     * @brief Construct a Numeric object with custom formatter and input
     * handler.
     *
     * @tparam Formatter formatter type, see formatter_check for more details.
     * @tparam InputHandler Input handler type, see input_handler_for_v
     * for more details.
     * @param name name of item
     * @param initial_value initial value
     * @param delta delta value
     * @param formatter formatter instance
     * @param handler handler instance
     */
    template <typename Formatter,
              typename InputHandler,
              enable_if_is_value_formatter<Formatter, item_type> = true,
              enable_if_is_input_handler<InputHandler, Numeric<TextSize, CharT, T>> = true>
    Numeric(StringView     name,
            T              initial_value,
            T              delta,
            Formatter&&    formatter,
            InputHandler&& handler) noexcept;

    /// execute the formatter
    constexpr sgl::error format(static_string<CharT, TextSize>& str, T val) noexcept;

    /// get delta value.
    constexpr T get_delta() const noexcept;

    /// set delta value.
    constexpr sgl::error set_delta(T delta) noexcept;

    /// get value stored.
    constexpr T get_value() const noexcept;

    /// set value
    constexpr sgl::error set_value(T value) noexcept;

    /// get format buffer
    constexpr static_string<CharT, TextSize>& get_buffer() noexcept;

  private:
    /// default input handler
    constexpr static sgl::error default_handle_input(item_type& numeric_item,
                                                     sgl::Input input) noexcept;

    constexpr static sgl::error default_format(static_string<CharT, TextSize>& text,
                                               T                               value) noexcept;

    Formatter_t                    format_{&default_format}; ///< formatter
    T                              value_{0};                ///< value
    T                              delta_{1};                ///< delta value
    static_string<CharT, TextSize> format_buffer_;           ///< format buffer
  };

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

  template <size_t TextSize, typename CharT, typename T>
  Numeric<TextSize, CharT, T>
      make_numeric(sgl::string_view<CharT> name, T initial_value, T delta) noexcept {
    return Numeric<TextSize, CharT, T>(name, initial_value, delta);
  }
  template <size_t TextSize, typename CharT, typename T, size_t N>
  Numeric<TextSize, CharT, T>
      make_numeric(const CharT (&name)[N], T initial_value, T delta) noexcept {
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
  constexpr Numeric<TextSize, char, T>
      make_numeric(const char (&name)[N], cx_arg<T, CxSize> initial_value, T delta) noexcept {
    return make_numeric<TextSize>(sgl::string_view<char>(name), initial_value, delta);
  }

  template <size_t TextSize,
            typename CharT,
            typename T,
            typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<TextSize, CharT, T>>>
  Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
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
  Numeric<TextSize, CharT, T> make_numeric(sgl::string_view<CharT> name,
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