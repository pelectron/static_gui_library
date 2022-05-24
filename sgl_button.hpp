#ifndef SGL_BUTTON_HPP
#define SGL_BUTTON_HPP
#include "sgl_item.hpp"
namespace sgl {
  /**
   * @brief
   *
   * @tparam LineWidth
   * @tparam CharT
   */
  template <size_t LineWidth, typename CharT>
  class Button_t : public Item_t<LineWidth, CharT> {
  public:
    using ClickHandler_t = Callable<sgl::error(Button_t<LineWidth, CharT>&)>;
    template <typename T>
    using click_handler_check = std::enable_if_t<
        std::is_invocable_r_v<sgl::error, T, Button_t<LineWidth, CharT>&>>;
    template <typename T>
    using input_handler_check =
        typename Item_t<LineWidth, CharT>::template input_handler_check<T>;

    Button_t(string_view<CharT> name_and_text)
        : Item_t(name_and_text,
                 &Button_t<LineWidth, CharT>::default_handle_input) {}

    Button_t(string_view<CharT> name, string_view<CharT> text)
        : Item_t<LineWidth, CharT>(
              name,
              text,
              &Button_t<LineWidth, CharT>::default_handle_input) {}

    template <typename ClickHandler,
              typename = click_handler_check<ClickHandler>>
    Button_t(string_view<CharT> name_and_text,
             ClickHandler&&           click_handler)
        : Item_t<LineWidth, CharT>(
              name_and_text,
              &Button_t<LineWidth, CharT>::default_handle_input),
          on_click(std::forward<ClickHandler>(click_handler)) {}

    template <typename ClickHandler,
              typename = click_handler_check<ClickHandler>>
    Button_t(string_view<CharT> name,
             string_view<CharT> text,
             ClickHandler&&           click_handler)
        : Item_t<LineWidth, CharT>(
              name,
              text,
              &Button_t<LineWidth, CharT>::default_handle_input),
          on_click(std::forward<ClickHandler>(click_handler)) {}

    template <typename ClickHandler,
              typename InputHandler,
              typename = click_handler_check<ClickHandler>,
              typename = input_handler_check<InputHandler>>
    Button_t(string_view<CharT> name,
             string_view<CharT> text,
             ClickHandler&&           click_handler,
             InputHandler&&           input_handler)
        : Item_t<LineWidth, CharT>(name,
                                   text,
                                   std::forward<InputHandler>(input_handler)),
          on_click(std::forward<ClickHandler>(click_handler)) {}

    template <typename ClickHandler,
              typename = click_handler_check<ClickHandler>>
    void set_click_handler(ClickHandler&& click_handler) {
      if constexpr (std::is_same_v<decltype(click_handler), ClickHandler_t>) {
        on_click = std::forward<ClickHandler>(click_handler);
      } else {
        on_click.bind(std::forward<ClickHandler>(click_handler));
      }
    }
    sgl::error click() { return on_click(*this); }

  private:
    static sgl::error default_handle_input(Item_t<LineWidth, CharT>& button,
                                           sgl::Input                input) {
      sgl::error ec = static_cast<Button_t<LineWidth, CharT>&>(button).click();
      switch (ec) {
        case sgl::error::no_error:
          return sgl::error::edit_finished;
        default:
          return ec;
      }
    }

    static sgl::error default_handle_click(Button_t<LineWidth, CharT>&) {
      return sgl::error::no_error;
    }

    ClickHandler_t on_click;
  };
} // namespace sgl
#endif