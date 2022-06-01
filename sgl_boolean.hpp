#ifndef SGL_BOOLEAN_HPP
#define SGL_BOOLEAN_HPP
#include "sgl_button.hpp"
namespace sgl {
  /// @cond
  namespace impl {
    template <typename CharT>
    static constexpr CharT True[] = {'T', 'R', 'U', 'E', '\0'};
    template <typename CharT>
    static constexpr CharT False[] = {'F', 'A', 'L', 'S', 'E', '\0'};
  }; // namespace impl
  /// @endcond

  /**
   * @brief This class implements a basic boolean item. When clicked, it will
   * toggle it's value and set it's text accordingly.
   * @details Boolean_t consists of the following data:
   * - boolean value: the value of this item
   * - 'true' string: the text to show when value == true.
   * - 'false' string: the text to show when value == false.
   *
   * @tparam LineWidth number of characters per line in the menu
   * @tparam CharT character type of the item
   */
  template <size_t LineWidth, typename CharT>
  class Boolean_t : public sgl::Button_t<LineWidth, CharT> {
  public:
    using StringView = typename sgl::Button_t<LineWidth, CharT>::StringView;
    template <typename T>
    using boolean_click_handler_check = std::enable_if_t<
        std::
            is_invocable_r_v<sgl::error, T, sgl::Boolean_t<LineWidth, CharT>&>>;

    /**
     * @brief Construct a boolean item, which toggles its text between "TRUE"
     * and "FALSE" when clicked.
     *
     * @param name name of the boolean item
     * @param initial_value initial value of the item
     */
    Boolean_t(StringView name, bool initial_value)
        : Button_t<LineWidth, CharT>(name,
                                     initial_value ? StringView(impl::True<CharT>)
                                                   : StringView(impl::False<CharT>),
                                     &default_handle_click),
          value_(initial_value) {}

    /**
     * @brief Construct a boolean item with custom true and false strings.
     * @param name name of item
     * @param value inital value of item
     * @param true_text text to set when value is true
     * @param false_text text to set when value is false
     */
    Boolean_t(StringView name,
              bool       value,
              StringView true_text,
              StringView false_text)
        : Button_t<LineWidth, CharT>(name,
                                     value ? true_text : false_text,
                                     &default_handle_click),
          value_(value), true_string_(true_text), false_string_(false_text) {}

    bool get_value() const { return value_; }

    sgl::error set_value(bool value) {
      value_ = value;
      if (value)
        this->set_text(true_string());
      else
        this->set_text(false_string());
      return sgl::error::no_error;
    }

    StringView true_string() const { return true_string_; }

    StringView false_string() const { return false_string_; }

  private:
    static sgl::error default_handle_click(Button_t<LineWidth, CharT>& button) {
      auto& b = static_cast<Boolean_t<LineWidth, CharT>&>(button);
      if (b.get_value()) {
        b.set_text(b.false_string());
        b.set_value(false);
      } else {
        b.set_text(b.true_string());
        b.set_value(true);
      }
      return sgl::error::no_error;
    }
    bool       value_{false};
    StringView true_string_{impl::True<CharT>};
    StringView false_string_{impl::False<CharT>};
  };

} // namespace sgl
#endif