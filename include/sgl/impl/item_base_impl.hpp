//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_ITEM_BASE_IMPL_HPP
#define SGL_IMPL_ITEM_BASE_IMPL_HPP
#include "sgl/item_base.hpp"

namespace sgl {
  template <typename ItemImpl, typename Traits>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(StringView text) noexcept : text_(text) {}

  template <typename ItemImpl, typename Traits>
  template <typename InputHandler, enable_if_is_input_handler<InputHandler, ItemImpl>>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(StringView text, InputHandler&& handler) noexcept
      : handler_(std::forward<InputHandler>(handler)), text_{text} {}

  template <typename ItemImpl, typename Traits>
  template <typename TickHandler, enable_if_is_tick_handler<TickHandler, ItemImpl>>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(StringView text, TickHandler&& handler) noexcept
      : tick_handler_(std::forward<TickHandler>(handler)), text_(text) {}

  template <typename ItemImpl, typename Traits>
  template <typename InputHandler,
            typename TickHandler,
            enable_if_is_input_handler<InputHandler, ItemImpl>,
            enable_if_is_tick_handler<TickHandler, ItemImpl>>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(StringView     text,
                                                 InputHandler&& input_handler,
                                                 TickHandler&&  tick_handler) noexcept
      : handler_(std::forward<InputHandler>(input_handler)),
        tick_handler_(std::forward<TickHandler>(tick_handler)), text_(text) {}

  template <typename ItemImpl, typename Traits>
  template <typename Menu>
  constexpr void ItemBase<ItemImpl, Traits>::set_menu(Menu* m) noexcept {
    static_cast<void>(m);
  }

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error ItemBase<ItemImpl, Traits>::handle_input(sgl::input input) noexcept {
    return handler_(*static_cast<item_type*>(this), input);
  }

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error ItemBase<ItemImpl, Traits>::set_text(StringView new_text) noexcept {
    text_ = new_text;
    return sgl::error::no_error;
  }

  template <typename ItemImpl, typename Traits>
  constexpr void ItemBase<ItemImpl, Traits>::clear_text() noexcept {
    text_.reset();
  }

  template <typename ItemImpl, typename Traits>
  template <typename InputHandler, enable_if_is_input_handler<InputHandler, ItemImpl>>
  constexpr typename ItemBase<ItemImpl, Traits>::item_type&
      ItemBase<ItemImpl, Traits>::set_input_handler(InputHandler&& handler) noexcept {
    static_assert(sgl::is_input_handler_for_v<InputHandler, item_type>,
                  "the provided handler is not a valid input handler.");
    handler_.bind(std::forward<InputHandler>(handler));
    return *static_cast<item_type*>(this);
  }

  template <typename ItemImpl, typename Traits>
  constexpr void ItemBase<ItemImpl, Traits>::tick() noexcept {
    tick_handler_(*static_cast<item_type*>(this));
  }

  template <typename ItemImpl, typename Traits>
  template <typename TickHandler, enable_if_is_tick_handler<TickHandler, ItemImpl>>
  constexpr typename ItemBase<ItemImpl, Traits>::item_type&
      ItemBase<ItemImpl, Traits>::set_tick_handler(TickHandler&& handler) noexcept {
    tick_handler_.bind(std::forward<TickHandler>(handler));
    return *static_cast<item_type*>(this);
  }

  template <typename ItemImpl, typename Traits>
  sgl::error ItemBase<ItemImpl, Traits>::default_handle_input(item_type&, sgl::input) noexcept {
    return sgl::error::edit_finished;
  }

} // namespace sgl
#endif /* SGL_IMPL_ITEM_BASE_IMPL_HPP */
