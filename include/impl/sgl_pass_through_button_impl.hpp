#ifndef SGL_PASS_THROUGH_BUTTON_HPP
#define SGL_PASS_THROUGH_BUTTON_HPP
#include "../sgl_pass_through_button.hpp"
namespace sgl {

  template <typename ItemImpl, typename Traits>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      StringView name_and_text) noexcept
      : Base(name_and_text) {}

  template <typename ItemImpl, typename Traits>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(StringView name,
                                                                   StringView text) noexcept
      : Base(name, text) {}

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      StringView     name,
      StringView     text,
      ClickHandler&& click_handler) noexcept
      : Base(name, text), click_handler_(std::forward<ClickHandler>(click_handler)) {}

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>,
            enable_if_is_tick_handler<TickHandler,
                                      typename PassThroughButton<ItemImpl, Traits>::item_type>>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      StringView     name,
      StringView     text,
      ClickHandler&& click_handler,
      TickHandler&&  tick_handler) noexcept
      : Base(name, text, std::forward<TickHandler>(tick_handler)),
        click_handler_(std::forward<ClickHandler>(click_handler)) {}

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            typename InputHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>,
            enable_if_is_input_handler<InputHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      StringView     name,
      StringView     text,
      ClickHandler&& click_handler,
      InputHandler&& input_handler) noexcept
      : Base(name, text, std::forward<InputHandler>(input_handler)),
        click_handler_(std::forward<ClickHandler>(click_handler)) {}

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>>
  void PassThroughButton<ItemImpl, Traits>::set_click_handler(
      ClickHandler&& click_handler) noexcept {
    static_assert(std::is_invocable_r_v<sgl::error, ClickHandler, item_type&>,
                  "the supplied click_handler does not meet the click "
                  "handler requirements");
    if constexpr (std::is_same_v<decltype(click_handler), ClickHandler_t>) {
      click_handler_ = std::forward<ClickHandler>(click_handler);
    } else {
      click_handler_.bind(std::forward<ClickHandler>(click_handler));
    }
  }

  template <typename ItemImpl, typename Traits>
  constexpr void PassThroughButton<ItemImpl, Traits>::set_click_handler(
      sgl::error (*click_handler)(item_type&) noexcept) noexcept {
    click_handler_.bind(click_handler);
  }

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error PassThroughButton<ItemImpl, Traits>::click() noexcept {
    return click_handler_(*static_cast<typename Traits::item_type*>(this));
  }
} // namespace sgl
#endif