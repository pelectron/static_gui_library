//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_PASS_THROUGH_BUTTON_IMPL_HPP
#define SGL_IMPL_PASS_THROUGH_BUTTON_IMPL_HPP
#include "sgl/pass_through_button.hpp"

namespace sgl {

  template <typename ItemImpl, typename Traits>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      typename PassThroughButton<ItemImpl, Traits>::StringView text) noexcept
      : Base(text, &PassThroughButton::button_input_handler) {}

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error PassThroughButton<ItemImpl, Traits>::click() noexcept {
    return click_handler_(*static_cast<typename Traits::item_type*>(this));
  }

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      typename PassThroughButton<ItemImpl, Traits>::StringView text,
      ClickHandler&&                                           click_handler) noexcept
      : Base(text, PassThroughButton::button_input_handler),
        click_handler_(std::forward<ClickHandler>(click_handler)) {}

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>,
            enable_if_is_tick_handler<TickHandler,
                                      typename PassThroughButton<ItemImpl, Traits>::item_type>>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      typename PassThroughButton<ItemImpl, Traits>::StringView text,
      ClickHandler&&                                           click_handler,
      TickHandler&&                                            tick_handler) noexcept
      : Base(text,
             PassThroughButton::button_input_handler,
             std::forward<TickHandler>(tick_handler)),
        click_handler_(std::forward<ClickHandler>(click_handler)) {}

  template <typename ItemImpl, typename Traits>
  template <typename ClickHandler,
            enable_if_is_click_handler<ClickHandler,
                                       typename PassThroughButton<ItemImpl, Traits>::item_type>>
  constexpr typename PassThroughButton<ItemImpl, Traits>::item_type&
      PassThroughButton<ItemImpl, Traits>::set_click_handler(
          ClickHandler&& click_handler) noexcept {
    static_assert(std::is_invocable_r_v<sgl::error, ClickHandler, item_type&>,
                  "the supplied click_handler does not meet the click "
                  "handler requirements");
    click_handler_.bind(std::forward<ClickHandler>(click_handler));
    return *static_cast<item_type*>(this);
  }

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error
      PassThroughButton<ItemImpl, Traits>::default_handle_click(item_type&) noexcept {
    return sgl::error::no_error;
  }

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error
      PassThroughButton<ItemImpl, Traits>::button_input_handler(item_type& item,
                                                                sgl::input) noexcept {
    sgl::error ec = item.click();
    switch (ec) {
      case sgl::error::no_error:
        return sgl::error::edit_finished;
      default:
        return ec;
    }
  }

} // namespace sgl
#endif /* SGL_IMPL_PASS_THROUGH_BUTTON_IMPL_HPP */
