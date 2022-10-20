//          Copyright Pele Constam 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SGL_PASS_THROUGH_BUTTON_HPP
#define SGL_PASS_THROUGH_BUTTON_HPP
#include "sgl/item_base.hpp"
namespace sgl {

  /// \brief This class can be inherited from if the item implementation needs a click handler.
  /// \tparam ItemImpl
  /// \tparam Traits
  template <typename ItemImpl, typename Traits = sgl::ItemTraits<ItemImpl>>
  class PassThroughButton : public sgl::ItemBase<ItemImpl, Traits> {
  public:
    /// base class of this item
    using Base = sgl::ItemBase<ItemImpl, Traits>;
    /// concrete item type
    using item_type = typename Base::item_type;
    /// string_View type of this item
    using StringView = typename Base::StringView;
    /// concrete type used for storing click handlers.
    using ClickHandler_t = Callable<sgl::error(item_type&)>;

    /// \brief create button with text
    /// \param text text of item
    explicit constexpr PassThroughButton(StringView text) noexcept;

    /// \brief Construct a Button with name, text, and click handler.
    /// \param text text of item
    /// \param click_handler click handler
    /// \tparam ClickHandler click handler type
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, item_type> = true>
    constexpr PassThroughButton(StringView text, ClickHandler&& click_handler) noexcept
        : Base(text, PassThroughButton::button_input_handler),
          click_handler_(std::forward<ClickHandler>(click_handler)) {}

    /// \brief Construct a Button with name, text, click handler and tick handler.
    /// The click handler is called every time the item is clicked.
    /// The tick handler is called on every tick of the menu.
    /// \param text text of item
    /// \param click_handler click handler
    /// \param tick_handler tick handler
    /// \tparam ClickHandler click handler type
    /// \tparam TickHandler tick handler type

    template <typename ClickHandler,
              typename TickHandler,
              enable_if_is_click_handler<ClickHandler, item_type> = true,
              enable_if_is_tick_handler<TickHandler, item_type> = true>
    constexpr PassThroughButton(StringView     text,
                                ClickHandler&& click_handler,
                                TickHandler&&  tick_handler) noexcept
        : Base(text,
               PassThroughButton::button_input_handler,
               std::forward<TickHandler>(tick_handler)),
          click_handler_(std::forward<ClickHandler>(click_handler)) {}

    /// \brief Set the click handler
    /// \tparam ClickHandler click handler type
    /// \param click_handler custom click handler
    /// \return item_type&
    template <typename ClickHandler, enable_if_is_click_handler<ClickHandler, item_type> = true>
    constexpr item_type& set_click_handler(ClickHandler&& click_handler) noexcept {
      static_assert(std::is_invocable_r_v<sgl::error, ClickHandler, item_type&>,
                    "the supplied click_handler does not meet the click "
                    "handler requirements");
      click_handler_.bind(std::forward<ClickHandler>(click_handler));
      return *static_cast<item_type*>(this);
    }

    /// execute click handler
    /// \return sgl::error
    constexpr sgl::error click() noexcept;

  private:
    constexpr static sgl::error default_handle_click(item_type&) noexcept {
      return sgl::error::no_error;
    }

    constexpr static sgl::error button_input_handler(item_type& item, sgl::Input) noexcept {
      sgl::error ec = item.click();
      switch (ec) {
        case sgl::error::no_error:
          return sgl::error::edit_finished;
        default:
          return ec;
      }
    }

    ClickHandler_t click_handler_{&default_handle_click};
  };

} // namespace sgl
#include "sgl/impl/pass_through_button_impl.hpp"
#endif /* SGL_PASS_THROUGH_BUTTON_HPP */
