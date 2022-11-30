//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_PASS_THROUGH_BUTTON_HPP
#define SGL_PASS_THROUGH_BUTTON_HPP
#include "sgl/item_base.hpp"

namespace sgl {
  /**
    This class can be inherited from if an item needs a button like behaviour. For an
    example on how to use, see sgl::Button.
    @tparam ItemImpl item type derived from PassThroughButton.
    @tparam Traits traits for the item type.
   */
  template <typename ItemImpl, typename Traits = sgl::ItemTraits<ItemImpl>>
  class PassThroughButton : public sgl::ItemBase<ItemImpl, Traits> {
  public:
    /// base class of this item
    using Base = sgl::ItemBase<ItemImpl, Traits>;

    /// concrete item type
    using item_type = typename Base::item_type;

    using char_type = typename Base::char_type;
    /// string_View type of this item
    using StringView = typename Base::StringView;

    /// concrete type used for storing click handlers.
    using ClickHandler_t = Callable<sgl::error(ItemImpl&)>;

    /**
      create button with text
      @param text text of item
     */
    explicit constexpr PassThroughButton(sgl::string_view<char_type> text) noexcept;

    /**
      Construct a Button with name, text, and click handler.
      @param text text of item
      @param click_handler click handler
      @tparam ClickHandler click handler type
     */
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, ItemImpl> = true>
    constexpr PassThroughButton(sgl::string_view<char_type> text, ClickHandler&& click_handler) noexcept;

    /**
      Construct a Button with name, text, click handler and tick handler.
      The click handler is called every time the item is clicked.
      The tick handler is called on every tick of the menu.
      @param text text of item
      @param click_handler click handler
      @param tick_handler tick handler
      @tparam ClickHandler click handler type
      @tparam TickHandler tick handler type
     */
    template <typename ClickHandler,
              typename TickHandler,
              enable_if_is_click_handler<ClickHandler, ItemImpl> = true,
              enable_if_is_tick_handler<TickHandler, ItemImpl> = true>
    constexpr PassThroughButton(sgl::string_view<char_type>     text,
                                ClickHandler&& click_handler,
                                TickHandler&&  tick_handler) noexcept;

    /**
      Set the click handler
      @tparam ClickHandler click handler type
      @param click_handler custom click handler
      @return ItemImpl&
     */
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, ItemImpl> = true>
    constexpr ItemImpl& set_click_handler(ClickHandler&& click_handler) noexcept;

    /**
      execute click handler
      @return sgl::error
     */
    constexpr sgl::error click() noexcept;

  private:
    constexpr static sgl::error default_handle_click(ItemImpl&) noexcept;
    constexpr static sgl::error button_input_handler(ItemImpl& item, sgl::input) noexcept;

    ClickHandler_t click_handler_{&default_handle_click};
  };

} // namespace sgl

#include "sgl/impl/pass_through_button_impl.hpp"
#endif /* SGL_PASS_THROUGH_BUTTON_HPP */
