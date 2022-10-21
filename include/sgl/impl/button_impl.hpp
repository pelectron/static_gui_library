//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_IMPL_BUTTON_IMPL_HPP
#define SGL_IMPL_BUTTON_IMPL_HPP
#include "sgl/button.hpp"

namespace sgl {
  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT>::Button(StringView text) noexcept : Base(text) {}

  template <size_t TextSize, typename CharT>
  template <typename ClickHandler,
            sgl::enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT>::Button(StringView text, ClickHandler&& click_handler) noexcept
      : Base(text, std::forward<ClickHandler>(click_handler)) {}

  template <size_t TextSize, typename CharT>
  template <typename TickHandler, enable_if_is_tick_handler<TickHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT>::Button(StringView text, TickHandler&& tick_handler) noexcept
      : Base(text, std::forward<TickHandler>(tick_handler)) {}

  template <size_t TextSize, typename CharT>
  template <typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>,
            enable_if_is_tick_handler<TickHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT>::Button(StringView     text,
                                            ClickHandler&& click_handler,
                                            TickHandler&&  tick_handler) noexcept
      : Base(text,
             std::forward<ClickHandler>(click_handler),
             std::forward<TickHandler>(tick_handler)) {}

  template <typename String>
  constexpr auto make_button(const String& text) {
    return Button(text);
  }

  template <size_t N,
            typename CharT,
            typename ClickHandler,
            enable_if_is_click_handler<ClickHandler, Button<N - 1, CharT>>>
  constexpr Button<N - 1, CharT> make_button(const CharT (&text)[N], ClickHandler&& handler) {
    return Button(text, std::forward<ClickHandler>(handler));
  }

  template <size_t TextSize,
            typename CharT,
            typename ClickHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT> make_button(sgl::string_view<CharT> text,
                                                ClickHandler&&          handler) {
    return Button<TextSize, CharT>(text, std::forward<ClickHandler>(handler));
  }

  template <size_t N,
            typename CharT,
            typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler, Button<N - 1, CharT>>,
            enable_if_is_tick_handler<TickHandler, Button<N - 1, CharT>>>
  constexpr Button<N - 1, CharT> make_button(const CharT (&text)[N],
                                             ClickHandler&& click_handler,
                                             TickHandler&&  tick_handler) {
    return Button(text,
                  std::forward<ClickHandler>(click_handler),
                  std::forward<TickHandler>(tick_handler));
  }

  template <size_t TextSize,
            typename CharT,
            typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>>,
            enable_if_is_tick_handler<TickHandler, Button<TextSize, CharT>>>
  constexpr Button<TextSize, CharT> make_button(sgl::string_view<CharT> text,
                                                ClickHandler&&          click_handler,
                                                TickHandler&&           tick_handler) {
    return Button<TextSize, CharT>(text,
                                   std::forward<ClickHandler>(click_handler),
                                   std::forward<TickHandler>(tick_handler));
  }
} // namespace sgl
#endif /* SGL_IMPL_BUTTON_IMPL_HPP */
