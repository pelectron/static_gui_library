#ifndef SGL_BUTTON_HPP
#define SGL_BUTTON_HPP
#include "sgl_item.hpp"
namespace sgl {
  /**
   * @brief This class implements a basic button item.
   *
   * @tparam LineWidth number of characters per line in the menu
   * @tparam CharT character type of the item
   */
  template <size_t LineWidth, typename CharT>
  class Button_t : public Item_t<LineWidth, CharT> {
  public:
    using Base = Item_t<LineWidth, CharT>;
    /// concrete type used for storing click handlers.
    using ClickHandler_t = Callable<sgl::error(Button_t<LineWidth, CharT>&)>;
    /**
     * @brief SFINAE check if F is invocable with a signature of
     * **sgl::error(Button_t<LineWidth, CharT>&)**.
     * @tparam F invocable type
     */
    template <typename F>
    using click_handler_check = std::enable_if_t<
        std::is_invocable_r_v<sgl::error, F, Button_t<LineWidth, CharT>&>>;

    /**
     * @brief input handler check of base.
     * @tparam F
     */
    template <typename F>
    using input_handler_check = typename Base::template input_handler_check<F>;

    using StringView = typename Base::StringView;

    /**
     * @brief create button with name and text
     * @{
     */
    constexpr Button_t(StringView name_and_text)
        : Base(name_and_text, &default_handle_input) {}

    constexpr Button_t(StringView name, StringView text)
        : Base(name, text, &default_handle_input) {}
    /// @}

    /**
     * @brief Construct a Button with name, text, and click handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     */
    constexpr Button_t(StringView name,
                       StringView text,
                       sgl::error (*click_handler)(Button_t<LineWidth, CharT>&))
        : Base(name, text, &default_handle_input),
          click_handler_(click_handler) {}

    /**
     * @brief Construct a Button with name, text, and click handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @tparam ClickHandler click handler type
     */
    template <typename ClickHandler>
    Button_t(StringView name, StringView text, ClickHandler&& click_handler)
        : Base(name, text, &default_handle_input),
          click_handler_(std::forward<ClickHandler>(click_handler)) {
      static_assert(std::is_invocable_r_v<sgl::error,
                                          ClickHandler,
                                          sgl::Button_t<LineWidth, CharT>&>,
                    "the supplied click_handler does not meet the click "
                    "handler requirements");
    }

    /**
     * @brief Construct a Button with name, text, and custom click and input
     * handler.
     * @tparam ClickHandler click handler type
     * @tparam InputHandler input handler type
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @param input_handler input handler
     */
    template <typename ClickHandler,
              typename InputHandler,
              click_handler_check<ClickHandler>* = nullptr,
              input_handler_check<InputHandler>* = nullptr>
    Button_t(StringView     name,
             StringView     text,
             ClickHandler&& click_handler,
             InputHandler&& input_handler)
        : Base(name, text, std::forward<InputHandler>(input_handler)),
          click_handler_(std::forward<ClickHandler>(click_handler)) {}
    /**
     * @brief Construct a Button with name, text, and custom click and input
     * handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @param input_handler input handler
     */
    constexpr Button_t(StringView name,
                       StringView text,
                       sgl::error (*click_handler)(Button_t<LineWidth, CharT>&),
                       sgl::error (*input_handler)(Item_t<LineWidth, CharT>&,
                                                   sgl::Input))
        : Base(name, text, input_handler), click_handler_(click_handler) {}

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
    static sgl::error default_handle_input(Base& button, sgl::Input /*input*/) {
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

} // namespace sgl
#endif