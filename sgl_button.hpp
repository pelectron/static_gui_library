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

    Button_t(string_view<CharT> item_name, string_view<CharT> text)
        : Item_t<LineWidth, CharT>(
              item_name,
              text,
              &Button_t<LineWidth, CharT>::default_handle_input) {}

    template <typename ClickHandler>
    Button_t(string_view<CharT> item_name,
             string_view<CharT> text,
             ClickHandler&&     click_handler)
        : Item_t<LineWidth, CharT>(
              item_name,
              text,
              &Button_t<LineWidth, CharT>::default_handle_input),
          click_handler_(std::forward<ClickHandler>(click_handler)) {
      static_assert(std::is_invocable_r_v<sgl::error,
                                          ClickHandler,
                                          sgl::Button_t<LineWidth, CharT>&>,
                    "the supplied click_handler does not meet the click "
                    "handler requirements");
    }

    template <typename ClickHandler,
              typename InputHandler,
              typename = click_handler_check<ClickHandler>,
              typename = input_handler_check<InputHandler>>
    Button_t(string_view<CharT> item_name,
             string_view<CharT> text,
             ClickHandler&&     click_handler,
             InputHandler&&     input_handler)
        : Item_t<LineWidth, CharT>(item_name,
                                   text,
                                   std::forward<InputHandler>(input_handler)),
          click_handler_(std::forward<ClickHandler>(click_handler)) {}

    template <typename ClickHandler>
    void set_click_handler(ClickHandler&& click_handler) {
      static_assert(std::is_invocable_r_v<sgl::error,
                                          ClickHandler,
                                          sgl::Button_t<LineWidth, CharT>&>,
                    "the supplied click_handler does not meet the click "
                    "handler requirements");
      if constexpr (std::is_same_v<decltype(click_handler), ClickHandler_t>) {
        click_handler_ = std::forward<ClickHandler>(click_handler);
      } else {
        click_handler_.bind(std::forward<ClickHandler>(click_handler));
      }
    }

    // execute click handler
    sgl::error click() { return click_handler_(*this); }

  private:
    static sgl::error default_handle_input(Item_t<LineWidth, CharT>& button,
                                           sgl::Input /*input*/) {
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

    ClickHandler_t click_handler_{&default_handle_click};
  };

  template <size_t LineWidth, typename CharT>
  struct is_item<Button_t<LineWidth, CharT>> : std::true_type {};
} // namespace sgl
#endif