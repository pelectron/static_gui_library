#ifndef SGL_BUTTON_IMPL_HPP
#define SGL_BUTTON_IMPL_HPP
#include "../sgl_button.hpp"
namespace sgl {
  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(StringView name_and_text) noexcept
      : Base(name_and_text) {}

  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(StringView name, StringView text) noexcept
      : Base(name, text) {}

  template <size_t TextSize, typename CharT>
  template <typename ClickHandler,
            sgl::enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT>::Button(StringView     name,
                                            StringView     text,
                                            ClickHandler&& click_handler) noexcept
      : Base(name, text, forward<ClickHandler>(click_handler)) {}

  template <size_t TextSize, typename CharT>
  template <typename ClickHandler,
            typename InputHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>,
            enable_if_is_input_handler<InputHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT>::Button(StringView     name,
                                            StringView     text,
                                            ClickHandler&& click_handler,
                                            InputHandler&& input_handler) noexcept
      : Base(name,
             text,
             forward<ClickHandler>(click_handler),
             forward<InputHandler>(input_handler)) {}

} // namespace sgl
#endif