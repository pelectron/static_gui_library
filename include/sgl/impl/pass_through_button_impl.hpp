#ifndef SGL_PASS_THROUGH_BUTTON_HPP
#define SGL_PASS_THROUGH_BUTTON_HPP
#include "sgl/pass_through_button.hpp"
namespace sgl {

  template <typename ItemImpl, typename Traits>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(StringView text) noexcept
      : Base(text,PassThroughButton::button_input_handler) {}

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error PassThroughButton<ItemImpl, Traits>::click() noexcept {
    return click_handler_(*static_cast<typename Traits::item_type*>(this));
  }
} // namespace sgl
#endif