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
    using Formatter_t =
        Callable<sgl::error(StaticString<CharT, LineWidth>&, T)>;
    using string_view_t = typename sgl::Item_t<LineWidth, CharT>::string_view_t;
    template <typename Formatter>
    Integer_t(string_view_t item_name,
              T             initial_value,
              T             delta,
              Formatter&&   formatter)
        : sgl::Item_t<LineWidth, CharT>(item_name, &default_handle_input),
          format_(std::forward<Formatter>(formatter)), value_(initial_value),
          delta_(delta) {
      this->clear_text();
      if (sgl::error::no_error == format_(format_buffer_, initial_value)) {
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
    }

    Integer_t(string_view_t item_name, T initial_value, T delta)
        : sgl::Item_t<LineWidth, CharT>(item_name, &default_handle_input),
          value_(initial_value), delta_(delta) {
      this->clear_text();
      if (sgl::error::no_error == format(format_buffer_, initial_value)) {
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
    }
    sgl::error format(StaticString<CharT, LineWidth>& str, T val) {
      return format_(str, val);
    }

    T          get_delta() const { return delta_; }
    sgl::error set_delta(T delta) { delta_ = delta; }
    T          get_value() const { return value_; }
    sgl::error set_value(T value) {
      sgl::error ec = format_(format_buffer_, value);
      if (ec == sgl::error::no_error) {
        value_ = value;
        this->set_text(format_buffer_);
      }
      format_buffer_.reset();
      return ec;
    }
    StaticString<CharT, LineWidth>& buffer() { return format_buffer_; }

  private:
    static sgl::error default_handle_input(sgl::Item_t<LineWidth, CharT>& num,
                                           sgl::Input input) {
      auto& numeric = static_cast<Integer_t<T, LineWidth, CharT>&>(num);
      if (!is_keyboard_input(input)) {
        switch (input) {
          case sgl::Input::up:
            [[fallthrough]];
          case sgl::Input::right:
            numeric.set_value(numeric.get_value() + numeric.get_delta());
            break;
          case sgl::Input::down:
            [[fallthrough]];
          case sgl::Input::left:
            numeric.set_value(numeric.get_value() - numeric.get_delta());
            break;
          default:
            break;
        }
      }
      return sgl::error::no_error;
    }

    Formatter_t                    format_{&default_format};
    T                              value_{0};
    T                              delta_{1};
    StaticString<CharT, LineWidth> format_buffer_;
  };
  /// @}
} // namespace sgl
#endif