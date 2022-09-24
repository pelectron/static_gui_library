#ifndef SGL_IMPL_PASS_THROUGH_BUTTON_IMPL_HPP
#define SGL_IMPL_PASS_THROUGH_BUTTON_IMPL_HPP
#include "sgl/pass_through_button.hpp"
namespace sgl {

  template <typename ItemImpl, typename Traits>
  constexpr PassThroughButton<ItemImpl, Traits>::PassThroughButton(
      typename PassThroughButton<ItemImpl, Traits>::StringView text) noexcept
      : Base(text, PassThroughButton::button_input_handler) {}

  template <typename ItemImpl, typename Traits>
  constexpr sgl::error PassThroughButton<ItemImpl, Traits>::click() noexcept {
    return click_handler_(*static_cast<typename Traits::item_type*>(this));
  }
} // namespace sgl
#endif /* SGL_IMPL_PASS_THROUGH_BUTTON_IMPL_HPP */
