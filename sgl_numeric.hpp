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
  static sgl::error default_format(StaticString<CharT, LineWidth>& str, T val) {
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
   * @addtogroup item_types Item Types
   * @{
   * @brief
   *
   *
   * @tparam T
   * @tparam LineWidth
   * @tparam CharT
   */
  template <typename T, size_t LineWidth, typename CharT>
  class Integer_t : public sgl::Item_t<LineWidth, CharT> {
  public:
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    static_assert(!std::is_same_v<bool, T>,
                  "T must not be bool. Use Boolean_t for a boolean item.");
    /// concrete formatter type
    using Formatter_t =
        Callable<sgl::error(StaticString<CharT, LineWidth>&, T)>;

    using string_view_t = typename sgl::Item_t<LineWidth, CharT>::string_view_t;

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
     * have the following function call signature: sgl::error(Integer_t<T,
     * LineWidth, CharT>&, sgl::Input).
     *
     * @tparam H handler type
     */
    template <typename H>
    using integer_handler_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error,
                                               H,
                                               Integer_t<T, LineWidth, CharT>&,
                                               sgl::Input>>;

    /**
     * @brief Construct a new Integer_t with default formatter and input
     * handling.
     *
     * @param item_name name of item
     * @param initial_value initial value
     * @param delta delta value
     */
    Integer_t(string_view_t item_name, T initial_value, T delta)
        : sgl::Item_t<LineWidth, CharT>(
              item_name,
              [](sgl::Item_t<LineWidth, CharT>& item, sgl::Input i) {
                return default_handle_input(
                    static_cast<Integer_t<T, LineWidth, CharT>&>(item),
                    i);
              }),
          format_(&default_format<CharT, LineWidth>), value_(initial_value),
          delta_(delta) {
      this->clear_text();
      if (sgl::error::no_error == format(format_buffer_, initial_value)) {
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
    }

    /**
     * @brief Construct a new Integer_t with custom formatter, but default input
     * handling.
     *
     * @tparam Formatter formatter type, see formatter_check for more details.
     * @param item_name name of item
     * @param initial_value inital value
     * @param delta delta value
     * @param formatter formatter instance
     */
    template <typename Formatter, formatter_check<Formatter>* = nullptr>
    Integer_t(string_view_t item_name,
              T             initial_value,
              T             delta,
              Formatter&&   formatter)
        : sgl::Item_t<LineWidth, CharT>(
              item_name,
              [](sgl::Item_t<LineWidth, CharT>& item, sgl::Input i) {
                return default_handle_input(
                    static_cast<Integer_t<T, LineWidth, CharT>&>(item),
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

    /**
     * @brief Construct a Integer_t object with custom formatter and input
     * handler.
     *
     * @tparam Formatter formatter type, see formatter_check for more details.
     * @tparam IntegerInputHandler Input handler type, see integer_handler_check
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
              integer_handler_check<IntegerInputHandler>* = nullptr>
    Integer_t(string_view_t         item_name,
              T                     initial_value,
              T                     delta,
              Formatter&&           formatter,
              IntegerInputHandler&& handler)
        : sgl::Item_t<LineWidth, CharT>(
              item_name,
              [handle = std::move(handler)](sgl::Item_t<LineWidth, CharT>& item,
                                            sgl::Input                     i) {
                return handle(
                    static_cast<Integer_t<T, LineWidth, CharT>&>(item),
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
    static sgl::error default_handle_input(Integer_t<T, LineWidth, CharT>& item,
                                           sgl::Input input) {
      if (!is_keyboard_input(input)) {
        switch (input) {
          case sgl::Input::up:
            [[fallthrough]];
          case sgl::Input::right:
            item.set_value(item.get_value() + item.get_delta());
            break;
          case sgl::Input::down:
            [[fallthrough]];
          case sgl::Input::left:
            item.set_value(item.get_value() - item.get_delta());
            break;
          default:
            break;
        }
      }
      return sgl::error::no_error;
    }

    Formatter_t                    format_{&default_format<CharT, LineWidth>};
    T                              value_{0};
    T                              delta_{1};
    StaticString<CharT, LineWidth> format_buffer_;
  };
  /// @}
} // namespace sgl
#endif