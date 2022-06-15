#ifndef SGL_ITEM_BASE_IMPL_HPP
#define SGL_ITEM_BASE_IMPL_HPP
#include "../sgl_item_base.hpp"
namespace sgl{
      template <typename ItemImpl, typename Traits>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(StringView name, StringView text) noexcept
      : name_(name), text_(text) {}

  template <typename ItemImpl, typename Traits>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(StringView name_and_text) noexcept
      : name_(name_and_text), text_(name_and_text) {}

  template <typename ItemImpl, typename Traits>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(
      StringView name_and_text,
      sgl::error (*handler)(item_type&, sgl::Input) noexcept) noexcept
      : handler_(handler), name_(name_and_text), text_(name_and_text) {}

  template <typename ItemImpl, typename Traits>
  constexpr ItemBase<ItemImpl, Traits>::ItemBase(
      StringView name,
      StringView text,
      sgl::error (*handler)(item_type&, sgl::Input) noexcept) noexcept
      : handler_(handler), name_(name), text_(text) {}

  template <typename ItemImpl, typename Traits>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, ItemImpl>>
  ItemBase<ItemImpl, Traits>::ItemBase(StringView name_and_text, InputHandler&& handler) noexcept
      : handler_(forward<InputHandler>(handler)), name_(name_and_text), text_{name_and_text} {}

  template <typename ItemImpl, typename Traits>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, ItemImpl>>
  ItemBase<ItemImpl, Traits>::ItemBase(StringView     name,
                                       StringView     text,
                                       InputHandler&& handler) noexcept
      : handler_(forward<InputHandler>(handler)), name_(name), text_(text) {}

  template <typename ItemImpl, typename Traits>
  template <typename Menu>
  constexpr void ItemBase<ItemImpl, Traits>::set_menu(Menu*) noexcept {}

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error ItemBase<ItemImpl, Traits>::handle_input(sgl::Input input) noexcept {
    return handler_(*static_cast<item_type*>(this), input);
  }

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error ItemBase<ItemImpl, Traits>::set_text(StringView new_text) noexcept {
    text_ = new_text;
    return sgl::error::no_error;
  }

  template <typename ItemImpl, typename Traits>
  constexpr const typename ItemBase<ItemImpl, Traits>::String&
      ItemBase<ItemImpl, Traits>::get_text() const noexcept {
    return text_;
  }

  template <typename ItemImpl, typename Traits>
  constexpr typename ItemBase<ItemImpl, Traits>::String&
      ItemBase<ItemImpl, Traits>::get_text() noexcept {
    return text_;
  }

  template <typename ItemImpl, typename Traits>
  constexpr typename ItemBase<ItemImpl, Traits>::StringView
      ItemBase<ItemImpl, Traits>::name() const noexcept {
    return name_;
  }

  template <typename ItemImpl, typename Traits>
  constexpr void ItemBase<ItemImpl, Traits>::clear_text() noexcept {
    text_.reset();
  }

  template <typename ItemImpl, typename Traits>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, ItemImpl>>
  void ItemBase<ItemImpl, Traits>::set_input_handler(InputHandler&& handler) noexcept {
    static_assert(is_invocable_r_v<sgl::error, InputHandler, item_type&, sgl::Input>,
                  "the provided handler is not a valid input handler.");
    if constexpr (is_same_v<InputHandler, InputHandler_t>) {
      handler_ = handler;
    } else {
      handler_.bind(forward<InputHandler>(handler));
    }
  }

  template <typename ItemImpl, typename Traits>
  sgl::error ItemBase<ItemImpl, Traits>::default_handle_input(item_type& item,
                                                              sgl::Input) noexcept {
    if constexpr (clickable) {
      sgl::error ec = item.click();
      switch (ec) {
        case sgl::error::no_error:
          return sgl::error::edit_finished;
        default:
          return ec;
      }
    } else {
      return sgl::error::edit_finished;
    }
  }
}
#endif