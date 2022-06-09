#ifndef SGL_BASIC_NUMERIC_HPP
#define SGL_BASIC_NUMERIC_HPP
#include "sgl_format.hpp"
#include "sgl_item_base.hpp"
#include "item_traits.hpp"
namespace sgl {
  /**
   * @brief This class models a numeric item. It consists of a value of type T,
   * a delta value of type T and a formatter to format T's into strings.
   *
   * @tparam T value type
   * @tparam TextSize display width
   * @tparam CharT character type
   */
  template <typename T, size_t TextSize, typename CharT>
  class Numeric : public sgl::ItemBase<traits<Numeric<T, TextSize, CharT>>> {
  public:
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                  "T must be an integral type");
    static_assert(!std::is_same_v<bool, T>,
                  "T must not be bool. Use Boolean_t for a boolean item.");
    using item_type = Numeric<T, TextSize, CharT>;
    using Base = sgl::ItemBase<traits<item_type>>;

    using StringView = typename Base::StringView;
    using String = typename Base::String;
    /// concrete formatter type
    using Formatter_t = Callable<sgl::error(static_string<CharT, TextSize>&, T)>;

    static constexpr size_t text_size = TextSize;

    /**
     * @brief Construct a new Numeric with default formatter and default
     * input handling.
     * @param name name of item
     * @param initial_value initial value
     * @param delta delta value
     */
    Numeric(StringView name, T initial_value, T delta) noexcept;

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
              enable_if_is_input_handler<InputHandler, Numeric<T, TextSize, CharT>> = true>
    Numeric(StringView     name,
                  T              initial_value,
                  T              delta,
                  Formatter&&    formatter,
                  InputHandler&& handler) noexcept;

    /// execute the formatter
    sgl::error format(static_string<CharT, TextSize>& str, T val) noexcept;

    /// get delta value.
    T get_delta() const noexcept;

    /// set delta value.
    sgl::error set_delta(T delta) noexcept;

    /// get value stored.
    T get_value() const noexcept;

    /// set value
    sgl::error set_value(T value) noexcept;

    /// get format buffer
    static_string<CharT, TextSize>& get_buffer() noexcept;

  private:
    /// default input handler
    static sgl::error default_handle_input(item_type& numeric_item, sgl::Input input) noexcept;

    static sgl::error default_format(static_string<CharT, TextSize>& text, T value) noexcept;

    Formatter_t                   format_{&default_format}; ///< formatter
    T                             value_{0};                ///< value
    T                             delta_{1};                ///< delta value
    static_string<CharT, TextSize> format_buffer_;           ///< format buffer
  };

  template <typename T, size_t TextSize, typename CharT>
  Numeric<T, TextSize, CharT>::Numeric(StringView name,
                                                   T          initial_value,
                                                   T          delta) noexcept
      : Base(name, &default_handle_input), format_(&default_format), value_(initial_value),
        delta_(delta) {
    set_value(initial_value);
  }

  template <typename T, size_t TextSize, typename CharT>
  template <typename Formatter,
            enable_if_is_value_formatter<Formatter, Numeric<T, TextSize, CharT>>>
  Numeric<T, TextSize, CharT>::Numeric(StringView  name,
                                                   T           initial_value,
                                                   T           delta,
                                                   Formatter&& formatter) noexcept
      : Base(name, &default_handle_input), format_(std::forward<Formatter>(formatter)),
        value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <typename T, size_t TextSize, typename CharT>
  template <typename Formatter,
            typename InputHandler,
            enable_if_is_value_formatter<Formatter, Numeric<T, TextSize, CharT>>,
            enable_if_is_input_handler<InputHandler, Numeric<T, TextSize, CharT>>>
  Numeric<T, TextSize, CharT>::Numeric(StringView     name,
                                                   T              initial_value,
                                                   T              delta,
                                                   Formatter&&    formatter,
                                                   InputHandler&& handler) noexcept
      : Base(name, std::forward<InputHandler>(handler)),
        format_(std::forward<Formatter>(formatter)), value_(initial_value), delta_(delta) {
    set_value(initial_value);
  }

  template <typename T, size_t TextSize, typename CharT>
  sgl::error Numeric<T, TextSize, CharT>::format(static_string<CharT, TextSize>& str,
                                                       T val) noexcept {
    return format_(str, val);
  }

  template <typename T, size_t TextSize, typename CharT>
  T Numeric<T, TextSize, CharT>::get_delta() const noexcept {
    return delta_;
  }

  template <typename T, size_t TextSize, typename CharT>
  sgl::error Numeric<T, TextSize, CharT>::set_delta(T delta) noexcept {
    delta_ = delta;
  }

  template <typename T, size_t TextSize, typename CharT>
  T Numeric<T, TextSize, CharT>::get_value() const noexcept {
    return value_;
  }

  template <typename T, size_t TextSize, typename CharT>
  sgl::error Numeric<T, TextSize, CharT>::set_value(T value) noexcept {
    format_buffer_.resize(TextSize);
    sgl::error ec = format_(format_buffer_, value);
    if (ec == sgl::error::no_error) {
      value_ = value;
      this->set_text(format_buffer_);
    }
    format_buffer_.reset();
    return ec;
  }

  template <typename T, size_t TextSize, typename CharT>
  static_string<CharT, TextSize>& Numeric<T, TextSize, CharT>::get_buffer() noexcept {
    return format_buffer_;
  }

  template <typename T, size_t TextSize, typename CharT>
  sgl::error Numeric<T, TextSize, CharT>::default_handle_input(
      Numeric<T, TextSize, CharT>& numeric_item,
      sgl::Input                         input) noexcept {
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

  template <typename T, size_t TextSize, typename CharT>
  sgl::error Numeric<T, TextSize, CharT>::default_format(static_string<CharT, TextSize>& text,
                                                               T value) noexcept {
    return sgl::format(text.data(), text.size(), value, 6, sgl::format_t::fixed);
  }
} // namespace sgl
#endif