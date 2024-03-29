//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_BUTTON_HPP
#define SGL_BUTTON_HPP
#include "sgl/pass_through_button.hpp"

namespace sgl {

  /// @headerfile button.hpp "sgl/button.hpp"
  /// @ingroup item_types

  /**
    This class represents a button.
    A button can be clicked, upon which a customizable click handler is called.
    The button is clicked by calling it's handle_input() method.

    @tparam TextSize size of the text field
    @tparam CharT character type of the button
  */
  template <size_t TextSize, typename CharT>
  class Button : public sgl::PassThroughButton<Button<TextSize, CharT>> {
  public:
    /// base class of Button
    using Base = sgl::PassThroughButton<Button<TextSize, CharT>>;
    /// string_view type of this item
    using StringView = typename Base::StringView;

    /// create button with text
    /// @param text text of button
    explicit constexpr Button(StringView text) noexcept;

    /**
      Construct a Button with text and click handler.
      @param text text of button
      @param click_handler click handler. Get's invoked on every call to click().
      @tparam ClickHandler click handler type
    */
    template <typename ClickHandler,
              enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true>
    constexpr Button(StringView text, ClickHandler&& click_handler) noexcept;

    /**
      Construct button with text and tick handler
      @tparam TickHandler tick handler type
      @param text text of button
      @param tick_handler custom tick handler
    */
    template <typename TickHandler,
              enable_if_is_tick_handler<TickHandler, Button<TextSize, CharT>> = true>
    constexpr Button(StringView text, TickHandler&& tick_handler) noexcept;

    /**
      Construct a button with text, click handler and tick handler.
      The click handler is called every time the item is clicked.
      The tick handler is called on every tick of the menu.
      @param text text of item
      @param click_handler click handler. Get's invoked on every call to click().
      @param tick_handler tick handler. Get's invoked on every call to tick().
      @tparam ClickHandler click handler type
      @tparam TickHandler tick handler type
     */
    template <typename ClickHandler,
              typename TickHandler,
              enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true,
              enable_if_is_tick_handler<TickHandler, Button<TextSize, CharT>> = true>
    constexpr Button(StringView     text,
                     ClickHandler&& click_handler,
                     TickHandler&&  tick_handler) noexcept;
  };

  /// @cond

  template <size_t Size, typename CharT>
  Button(const CharT (&)[Size]) -> Button<Size - 1, CharT>;

  template <size_t Size, typename CharT, typename ClickHandler>
  Button(const CharT (&)[Size], ClickHandler&&) -> Button<Size - 1, CharT>;

  template <size_t Size, typename CharT, typename ClickHandler, typename TickHandler>
  Button(const CharT (&)[Size], ClickHandler&&, TickHandler&&) -> Button<Size - 1, CharT>;

  /// @endcond

  /// @ingroup item_factories
  /// @addtogroup button_factories
  /// @{

  /**
    create button with text

    @tparam Size size of character literal
    @tparam CharT character type
    @param text text to show, i.e. character literal
    @return Button<Size - 1, CharT>
  */
  template <size_t Size, typename CharT>
  constexpr Button<Size - 1, CharT> button(const CharT (&text)[Size]);

  /**
    create button from text

    @tparam TextSize size of text field of the button
    @tparam CharT character type
    @param text text to show
    @return Button<TextSize,CharT>
  */
  template <size_t TextSize, typename CharT>
  constexpr Button<TextSize, CharT> button(sgl::string_view<CharT> text);
  template <size_t Size,
            typename CharT,
            typename ClickHandler,
            enable_if_is_click_handler<ClickHandler, Button<Size - 1, CharT>> = true>
  constexpr Button<Size - 1, CharT> button(const CharT (&text)[Size], ClickHandler&& handler);

  /**
    create button with text and custom click handler

    @tparam TextSize size of text field of the button
    @tparam CharT character type
    @tparam ClickHandler click handler type
    @param text text to show
    @param handler custom click handler
    @return constexpr Button<TextSize, CharT>
  */
  template <size_t TextSize,
            typename CharT,
            typename ClickHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true>
  constexpr Button<TextSize, CharT> button(sgl::string_view<CharT> text, ClickHandler&& handler);

  /**
    create button with text, and custom click and tick handler.

    @tparam Size size of character literal
    @tparam CharT character type
    @tparam ClickHandler click handler type
    @tparam TickHandler tick handler type
    @param text text to show
    @param click_handler custom click handler
    @param tick_handler custom tick handler
    @return constexpr Button<Size - 1, CharT>
  */
  template <size_t Size,
            typename CharT,
            typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler, Button<Size - 1, CharT>> = true,
            enable_if_is_tick_handler<TickHandler, Button<Size - 1, CharT>> = true>
  constexpr Button<Size - 1, CharT>
      button(const CharT (&text)[Size], ClickHandler&& click_handler, TickHandler&& tick_handler);

  /**
    create button with text, and custom click and tick handler.

    @tparam TextSize size of text field of the button
    @tparam CharT character type
    @tparam ClickHandler click handler type
    @tparam TickHandler tick handler type
    @param text text to show
    @param click_handler custom click handler
    @param tick_handler custom tick handler
    @return constexpr Button<TextSize, CharT>
  */
  template <size_t TextSize,
            typename CharT,
            typename ClickHandler,
            typename TickHandler,
            enable_if_is_click_handler<ClickHandler, Button<TextSize, CharT>> = true,
            enable_if_is_tick_handler<TickHandler, Button<TextSize, CharT>> = true>
  constexpr Button<TextSize, CharT> button(sgl::string_view<CharT> text,
                                           ClickHandler&&          click_handler,
                                           TickHandler&&           tick_handler);
  /// @}
} // namespace sgl

#include "sgl/impl/button_impl.hpp"
#endif /* SGL_BUTTON_HPP */
