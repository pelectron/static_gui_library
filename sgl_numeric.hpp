#ifndef SGL_NUMERIC_HPP
#define SGL_NUMERIC_HPP
#include "sgl_config.h"
#include "sgl_item.hpp"
#if ENABLE_CHARCONV
  #include <charconv>
#endif
namespace sgl {
#if ENABLE_CHARCONV
  template <typename CharT, size_t LineWidth, typename T>
  static constexpr sgl::error
      default_format(StaticString<CharT, LineWidth>& str, T val) {
    str.resize(LineWidth);
    auto write_res =
        std::to_chars(str.begin(), str.begin() + LineWidth - 1, val);
    str.resize(write_res.ptr - str.begin());
    if (write_res.ec == std::errc())
      return sgl::error::no_error;
    else
      return sgl::error::format_error;
  }
#else
  template <typename CharT, size_t LineWidth, typename T>
  static sgl::error default_format(StaticString<CharT, LineWidth>& str, T val) {
    return sgl::error::no_error;
  }
#endif
  /**
   * @brief This class models a numeric item. It consists of a value of type T,
   * a delta value of type T and a formatter to format T's into strings.
   *
   * @tparam T value type
   * @tparam LineWidth display width
   * @tparam CharT character type
   */
  template <typename T, size_t LineWidth, typename CharT>
  class Numeric_t : public sgl::Item_t<LineWidth, CharT> {
  public:
    static_assert(std::is_integral_v<T> || std::is_floating_point_v<T>,
                  "T must be an integral type");
    static_assert(!std::is_same_v<bool, T>,
                  "T must not be bool. Use Boolean_t for a boolean item.");
    using Base = sgl::Item_t<LineWidth, CharT>;
    /// concrete formatter type
    using Formatter_t =
        Callable<sgl::error(StaticString<CharT, LineWidth>&, T)>;

    using StringView = typename Base::StringView;

    /**
     * @brief check if F is a valid formatter. An instance of F must have the
     * following call signature: sgl::error(StaticString<CharT, LineWidth>&, T).
     *
     * @tparam F formatter type
     */
    template <typename F>
    using formatter_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error,
                                               F,
                                               StaticString<CharT, LineWidth>&,
                                               T>>;

    /**
     * @brief checks if H is a valid integer input handler.An instance of H must
     * have the following function call signature: sgl::error(Numeric_t<T,
     * LineWidth, CharT>&, sgl::Input).
     *
     * @tparam H handler type
     */
    template <typename H>
    using numeric_handler_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error,
                                               H,
                                               Numeric_t<T, LineWidth, CharT>&,
                                               sgl::Input>>;

    /**
     * @brief Construct a new Numeric_t with default formatter and default input
     * handling.
     * @param item_name name of item
     * @param initial_value initial value
     * @param delta delta value
     */
    Numeric_t(StringView item_name, T initial_value, T delta)
        : Base(item_name, &default_handle_input),
          format_(&default_format<CharT, LineWidth>), value_(initial_value),
          delta_(delta) {
      this->clear_text();
      if (sgl::error::no_error == format(format_buffer_, initial_value)) {
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
    }

    /**
     * @brief Construct a new Numeric_t with custom formatter, but default input
     * handling.
     *
     * @tparam Formatter formatter type, see formatter_check for more details.
     * @param item_name name of item
     * @param initial_value inital value
     * @param delta delta value
     * @param formatter formatter instance
     */
    template <typename Formatter, formatter_check<Formatter>* = nullptr>
    Numeric_t(StringView  item_name,
              T           initial_value,
              T           delta,
              Formatter&& formatter)
        : Base(item_name, &default_handle_input),
          format_(std::forward<Formatter>(formatter)), value_(initial_value),
          delta_(delta) {
      this->clear_text();
      if (sgl::error::no_error == format(format_buffer_, initial_value)) {
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
    }

    /**
     * @brief Construct a Numeric_t object with custom formatter and input
     * handler.
     *
     * @tparam Formatter formatter type, see formatter_check for more details.
     * @tparam IntegerInputHandler Input handler type, see numeric_handler_check
     * for more details.
     * @param item_name name of item
     * @param initial_value initial value
     * @param delta delta value
     * @param formatter formatter instance
     * @param handler handler instance
     */
    template <typename Formatter,
              typename IntegerInputHandler,
              formatter_check<Formatter>* = nullptr,
              numeric_handler_check<IntegerInputHandler>* = nullptr>
    Numeric_t(StringView            item_name,
              T                     initial_value,
              T                     delta,
              Formatter&&           formatter,
              IntegerInputHandler&& handler)
        : Base(item_name,
               [handle = std::move(handler)](Base& item, sgl::Input i) {
                 return handle(
                     static_cast<Numeric_t<T, LineWidth, CharT>&>(item),
                     i);
               }),
          format_(std::forward<Formatter>(formatter)), value_(initial_value),
          delta_(delta) {
      this->clear_text();
      if (sgl::error::no_error == format(format_buffer_, initial_value)) {
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
    }

    /// execute the formatter
    sgl::error format(StaticString<CharT, LineWidth>& str, T val) {
      return format_(str, val);
    }

    /// get delta value.
    T get_delta() const { return delta_; }

    /// set delta value.
    sgl::error set_delta(T delta) { delta_ = delta; }

    /// get value stored.
    T get_value() const { return value_; }

    /// set value
    sgl::error set_value(T value) {
      sgl::error ec = format_(format_buffer_, value);
      if (ec == sgl::error::no_error) {
        value_ = value;
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
      return ec;
    }

    /// get format buffer
    StaticString<CharT, LineWidth>& get_buffer() { return format_buffer_; }

  private:
    /// default input handler
    static sgl::error default_handle_input(Base& item, sgl::Input input) {
      auto& num = static_cast<Numeric_t<T, LineWidth, CharT>&>(item);
      if (!is_keyboard_input(input)) {
        switch (input) {
          case sgl::Input::up:
            [[fallthrough]];
          case sgl::Input::right:
            num.set_value(num.get_value() + num.get_delta());
            break;
          case sgl::Input::down:
            [[fallthrough]];
          case sgl::Input::left:
            num.set_value(num.get_value() - num.get_delta());
            break;
          default:
            break;
        }
      }
      return sgl::error::no_error;
    }

    Formatter_t format_{&default_format<CharT, LineWidth, T>}; ///< formatter
    T           value_{0};                                     ///< value
    T           delta_{1};                                     ///< delta value
    StaticString<CharT, LineWidth> format_buffer_; ///< format buffer
  };
} // namespace sgl
#endif