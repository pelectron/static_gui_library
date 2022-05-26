#ifndef SGL_TEXT_HPP
#define SGL_TEXT_HPP
#include "sgl_item.hpp"
namespace sgl {
  template <typename CharT>
  constexpr bool is_visible_char(CharT c) {
    return (c >= ' ') and (c <= '}');
  }

  /**
   * @addtogroup item_types Item Types
   * @{
   * @brief
   *
   * @tparam LineWidth
   * @tparam CharT
   */
  template <size_t LineWidth, typename CharT>
  class Text_t : public sgl::Item_t<LineWidth, CharT> {
  public:
    using Validator_t = Callable<sgl::error(string_view<CharT>)>;
    using string_view_t = typename sgl::Item_t<LineWidth, CharT>::string_view_t;

    template <typename T>
    using validator_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error, T, string_view_t>>;

    template <typename T>
    using input_handler_check =
        typename Item_t<LineWidth, CharT>::template input_handler_check<T>;

    Text_t(string_view_t item_name, string_view_t text)
        : Item_t<LineWidth, CharT>(item_name, text, &default_handle_input),
          cursor_(text.size() - 1) {}

    template <typename Validator, typename = validator_check<Validator>>
    Text_t(string_view_t item_name, string_view_t text, Validator&& validate)
        : Item_t<LineWidth, CharT>(item_name, text, &default_handle_input),
          validate_(std::forward<Validator>(validate)),
          cursor_(text.size() - 1) {}

    template <typename Validator,
              typename InputHandler,
              typename = validator_check<Validator>,
              typename = input_handler_check<InputHandler>>
    Text_t(string_view_t  item_name,
           string_view_t  text,
           Validator&&    validate,
           InputHandler&& input_handler)
        : Item_t<LineWidth, CharT>(item_name,
                                   text,
                                   std::forward<InputHandler>(input_handler)),
          validate_(std::forward<Validator>(validate)),
          cursor_(text.size() - 1) {}
    error          validate(string_view_t str) { return validate_(str); }
    constexpr void increment_cursor() {
      cursor_ = (cursor_ == this->get_text().size()) ? cursor_ : cursor_ + 1;
    }

    constexpr void decrement_cursor() {
      cursor_ = (cursor_ == 0) ? 0 : cursor_ - 1;
    }

    constexpr size_t cursor() const { return cursor_; }

  private:
    static sgl::error default_handle_input(Item_t<LineWidth, CharT>& element,
                                           sgl::Input                input) {
      auto& text_elem = static_cast<Text_t<LineWidth, CharT>&>(element);
      StaticString<CharT, LineWidth> buffer =
          text_elem.get_text(); // copy of item text!

      if (is_keyboard_input(input)) {
        CharT c = get_char<CharT>(input);
        if (is_visible_char(c))
          buffer.append(c);
        // TODO: special characters like delete
      } else {
        switch (input) {
          case sgl::Input::up:
            if (is_visible_char(buffer[text_elem.cursor()] + 1)) {
              ++buffer[text_elem.cursor()];
            } else {
              buffer[text_elem.cursor()] = '~';
            }
            break;
          case sgl::Input::down:
            if (is_visible_char(buffer[text_elem.cursor()] - 1)) {
              --buffer[text_elem.cursor()];
            } else {
              buffer[text_elem.cursor()] = ' ';
            }
            break;
          case sgl::Input::left:
            text_elem.decrement_cursor();
            break;
          case sgl::Input::right:
            text_elem.increment_cursor();
            break;
          default:
            break;
        }
      }
      sgl::error ec = text_elem.validate(buffer);
      if (ec == error::no_error) {
        text_elem.set_text(buffer);
        return error::no_error;
      }
      return ec;
    }

    static sgl::error default_validate(string_view_t) {
      return sgl::error::no_error;
    };

    Validator_t validate_{&default_validate};
    size_t      cursor_{0};
  };

  /**
   * @brief
   *
   * @tparam LineWidth
   * @tparam CharT
   */
  template <size_t LineWidth, typename CharT>
  class ConstText_t : public Item_t<LineWidth, CharT> {
  public:
    using string_view_t = typename sgl::Item_t<LineWidth, CharT>::string_view_t;

    ConstText_t(string_view_t item_name, string_view_t text)
        : Item_t<LineWidth, CharT>(item_name, text) {}

    ConstText_t(string_view_t name_and_text)
        : Item_t<LineWidth, CharT>(name_and_text) {}
  };
  /// @}
} // namespace sgl
#endif