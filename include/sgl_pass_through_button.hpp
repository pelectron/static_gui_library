#ifndef SGL_BUTTON_PASS_THROUGH_HPP
#define SGL_BUTTON_PASS_THROUGH_HPP
#include "sgl_item_base.hpp"
namespace sgl {
  template <typename Traits>
  class PassThroughButton : public sgl::ItemBase<Traits> {
  public:
    using item_type = typename Traits::item_type;
    using Base = sgl::ItemBase<Traits>;
    using StringView = typename Base::StringView;
    /// concrete type used for storing click handlers.
    using ClickHandler_t = Callable<sgl::error(item_type&)>;

    /**
     * @brief create button with name and text
     * @{
     */
    constexpr PassThroughButton(StringView name_and_text) noexcept;
    constexpr PassThroughButton(StringView name, StringView text) noexcept;
    /// @}

    /**
     * @brief Construct a Button with name, text, and click handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     */
    constexpr PassThroughButton(StringView name,
                                StringView text,
                                sgl::error (*click_handler)(item_type&) noexcept) noexcept;

    /**
     * @brief Construct a Button with name, text, and click handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @tparam ClickHandler click handler type
     */
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, item_type> = true>
    PassThroughButton(StringView name, StringView text, ClickHandler&& click_handler) noexcept;

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
              enable_if_is_click_handler<ClickHandler, item_type> = true,
              enable_if_is_input_handler<InputHandler, item_type> = true>
    PassThroughButton(StringView     name,
                      StringView     text,
                      ClickHandler&& click_handler,
                      InputHandler&& input_handler) noexcept;
    /**
     * @brief Construct a Button with name, text, and custom click and input
     * handler.
     * @param name name of item
     * @param text text of item
     * @param click_handler click handler
     * @param input_handler input handler
     */
    constexpr PassThroughButton(StringView name,
                                StringView text,
                                sgl::error (*click_handler)(item_type&) noexcept,
                                sgl::error (*input_handler)(item_type&,
                                                            sgl::Input) noexcept) noexcept;
    /**
     * @brief Set the click handler
     * @tparam ClickHandler
     * @param click_handler
     */
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, item_type> = true>
    void           set_click_handler(ClickHandler&& click_handler) noexcept;
    constexpr void set_click_handler(sgl::error (*click_handler)(item_type&) noexcept) noexcept;

    // execute click handler
    sgl::error click() noexcept;

  private:
    static sgl::error default_handle_click(item_type&) noexcept { return sgl::error::no_error; }

    ClickHandler_t click_handler_{&default_handle_click};
  };

  template <typename Traits>
  constexpr PassThroughButton<Traits>::PassThroughButton(StringView name_and_text) noexcept
      : Base(name_and_text) {}

  template <typename Traits>
  constexpr PassThroughButton<Traits>::PassThroughButton(StringView name, StringView text) noexcept
      : Base(name, text) {}

  template <typename Traits>
  constexpr PassThroughButton<Traits>::PassThroughButton(
      StringView name,
      StringView text,
      sgl::error (*click_handler)(item_type&) noexcept) noexcept
      : Base(name, text), click_handler_(click_handler) {}

  template <typename Traits>
  constexpr PassThroughButton<Traits>::PassThroughButton(
      StringView name,
      StringView text,
      sgl::error (*click_handler)(item_type&) noexcept,
      sgl::error (*input_handler)(item_type&, sgl::Input) noexcept) noexcept
      : Base(name, text, input_handler), click_handler_(click_handler) {}

  template <typename Traits>
  template <typename ClickHandler,
            enable_if_is_click_handler<ClickHandler, typename Traits::item_type>>
  PassThroughButton<Traits>::PassThroughButton(StringView     name,
                                               StringView     text,
                                               ClickHandler&& click_handler) noexcept
      : Base(name, text), click_handler_(forward<ClickHandler>(click_handler)) {}

  template <typename Traits>
  template <typename ClickHandler,
            typename InputHandler,
            enable_if_is_click_handler<ClickHandler, typename Traits::item_type>,
            enable_if_is_input_handler<InputHandler, typename Traits::item_type>>
  PassThroughButton<Traits>::PassThroughButton(StringView     name,
                                               StringView     text,
                                               ClickHandler&& click_handler,
                                               InputHandler&& input_handler) noexcept
      : Base(name, text, forward<InputHandler>(input_handler)),
        click_handler_(forward<ClickHandler>(click_handler)) {}

  template <typename Traits>
  template <typename ClickHandler,
            enable_if_is_click_handler<ClickHandler, typename Traits::item_type>>
  void PassThroughButton<Traits>::set_click_handler(ClickHandler&& click_handler) noexcept {
    static_assert(is_invocable_r_v<sgl::error, ClickHandler, item_type&>,
                  "the supplied click_handler does not meet the click "
                  "handler requirements");
    if constexpr (is_same_v<decltype(click_handler), ClickHandler_t>) {
      click_handler_ = forward<ClickHandler>(click_handler);
    } else {
      click_handler_.bind(forward<ClickHandler>(click_handler));
    }
  }

  template <typename Traits>
  constexpr void PassThroughButton<Traits>::set_click_handler(
      sgl::error (*click_handler)(item_type&) noexcept) noexcept {
    click_handler_.bind(click_handler);
  }

  template <typename Traits>
  sgl::error PassThroughButton<Traits>::click() noexcept {
    return click_handler_(*static_cast<typename Traits::item_type*>(this));
  }
} // namespace sgl
#endif