#ifndef SGL_TEXT_HPP
#define SGL_TEXT_HPP
#include "sgl_item.hpp"
namespace sgl {
  template <typename CharT>
  constexpr bool is_visible_char(CharT c) {
    return (c >= ' ') and (c <= '}');
  }

  /**
   * @brief This class models a item with editable text.
   *
   * @tparam LineWidth display width
   * @tparam CharT character type
   */
  template <size_t LineWidth, typename CharT>
  class Text_t : public sgl::Item_t<LineWidth, CharT> {
  public:
    using Base = sgl::Item_t<LineWidth, CharT>;
    using Validator_t = Callable<sgl::error(string_view<CharT>)>;
    using StringView = typename Base::StringView;

    template <typename T>
    using validator_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error, T, StringView>>;

    template <typename T>
    using input_handler_check = typename Base::template input_handler_check<T>;

    /**
     * @brief Construct a new Text_t object
     * @param name name of item.
     * @param text text of item.
     */
    constexpr Text_t(StringView name, StringView text)
        : Base(name, text, &default_handle_input), cursor_(text.size() - 1) {}

    /**
     * @brief Construct a new Text_t object with custom validator.
     * @tparam Validator validator type. See validator_check and Validator_t for
     * more info.
     * @param name name of item.
     * @param text text of item.
     * @param validate validator instance.
     */
    template <typename Validator, typename = validator_check<Validator>>
    Text_t(StringView name, StringView text, Validator&& validate)
        : Base(name, text, &default_handle_input),
          validate_(std::forward<Validator>(validate)),
          cursor_(text.size() - 1) {}

    /**
     * @brief Construct a new Text_t object with custom validator.
     * @param name name of item.
     * @param text text of item.
     * @param validate validator instance.
     */
    constexpr Text_t(StringView name,
                     StringView text,
                     sgl::error (*validate)(string_view<CharT>))
        : Base(name, text, &default_handle_input), validate_(validate),
          cursor_(text.size() - 1) {}

    /**
     * @brief Construct a new Text_t object with custom validator and input
     * handler.
     * @tparam Validator validator type. See validator_check and Validator_t.
     * @tparam InputHandler input handler type. See input_handler_check and
     * InputHandler_t.
     * @param name name of item.
     * @param text text of item.
     * @param validate custom validator.
     * @param input_handler custom input handler.
     */
    template <typename Validator,
              typename InputHandler,
              typename = validator_check<Validator>,
              typename = input_handler_check<InputHandler>>
    Text_t(StringView     name,
           StringView     text,
           Validator&&    validate,
           InputHandler&& input_handler)
        : Base(name, text, std::forward<InputHandler>(input_handler)),
          validate_(std::forward<Validator>(validate)),
          cursor_(text.size() - 1) {}
    /**
     * @brief Construct a new Text_t object with custom validator and input
     * handler.
     * @param name name of item.
     * @param text text of item.
     * @param validate custom validator.
     * @param input_handler custom input handler.
     */
    constexpr Text_t(StringView name,
                     StringView text,
                     sgl::error (*validate)(string_view<CharT>),
                     sgl::error (*input_handler)(Item_t<LineWidth, CharT>&,
                                                 sgl::Input))
        : Base(name, text, input_handler), validate_(validate),
          cursor_(text.size() - 1) {}

    // invoke validator. A return value of sgl::error::no_error means the text in str is valid.
    error validate(StringView str) { return validate_(str); }

    constexpr void increment_cursor() {
      cursor_ = (cursor_ == this->get_text().size()) ? cursor_ : cursor_ + 1;
    }

    constexpr void decrement_cursor() {
      cursor_ = (cursor_ == 0) ? 0 : cursor_ - 1;
    }

    constexpr size_t cursor() const { return cursor_; }

  private:
    static sgl::error default_handle_input(Base& element, sgl::Input input) {
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

    constexpr static sgl::error default_validate(StringView) {
      return sgl::error::no_error;
    };

    Validator_t validate_{&default_validate};
    size_t      cursor_{0};
  };

  /**
   * @brief This class models an item with immutable text.
   *
   * @tparam LineWidth display width
   * @tparam CharT character type
   */
  template <size_t LineWidth, typename CharT>
  class ConstText_t : public Item_t<LineWidth, CharT> {
  public:
    using Base = sgl::Item_t<LineWidth, CharT>;
    using StringView = typename Base::StringView;

    /**
     * @brief Construct a ConstText_t with name and text.
     * @param name name of item.
     * @param text text of item.
     */
    constexpr ConstText_t(StringView name, StringView text) : Base(name, text) {}

    /**
     * @brief Construct a new ConstText_t with identical name and text.
     * @param name_and_text name and text of item.
     */
    constexpr ConstText_t(StringView name_and_text) : Base(name_and_text) {}
  };
} // namespace sgl
#endif