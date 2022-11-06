/**
 * \file item_base.hpp
 * \author Pelé Constam (pelectron1602@gmail.com)
 * \brief This files defines the item_base class template.
 * \version 0.1
 * @date 2022-09-26
 *
 *          Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef SGL_ITEM_BASE_HPP
#define SGL_ITEM_BASE_HPP
#include "sgl/callable.hpp"
#include "sgl/item_concepts.hpp"
#include "sgl/item_traits.hpp"
#include "sgl/static_string.hpp"
#include "sgl/string_view.hpp"

namespace sgl {
  /**
   * \headerfile item_base.hpp "sgl/item_base.hpp"
   * \brief CRTP base class for [Items](concepts.md#item).
   * \details An item  minimally consists of a text field with const and non const getters, and
   * methods for handling inputs and ticks.
   *
   * ItemBase can be inherited from to enable these features via the CRTP pattern. Besides the
   * minimal features of an [Item](concepts.md#item), ItemBase also provides means to change the
   * input and tick handling strategy at runtime and constexpr time.
   *
   * ItemBase has an sgl::Callable member for input and tick handling, which can be set with the
   * set_input_handler() and set_tick_handler() functions respectively. The installed handlers will
   * then be called when the items handle_input() and tick() methods are called. See also the
   * [input handling docs](markdown/input_handling.md) and 
   * [tick handling docs](markdown/external_updates.md).
   *
   * The text field contains the text which the item should show. It is a static_string, i.e. does
   * not heap allocate and has a fixed capacity.
   *
   *
   * \tparam ItemImpl concrete item type, i.e. the custom Item. ItemImpl must be derived from
   * ItemBase. This will be the item type the input and tick handler members accept.
   * \tparam Traits ItemTraits type for ItemImpl. This is used because accessing inner
   * typedefs of yet undefined classes cannot be done in C++.
   */
  template <typename ItemImpl, typename Traits = sgl::ItemTraits<ItemImpl>>
  class ItemBase {
  public:
    static_assert(sgl::detail::has_char_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'char_type'.");
    static_assert(sgl::detail::has_text_size_v<Traits>,
                  "Traits type needs a static constexpr member of type size_t called 'text_size'.");
    /// traits of this item
    using traits_type = Traits;
    /// concrete item type.
    using item_type = ItemImpl;
    /// character type of the item.
    using char_type = typename traits_type::char_type;
    /// text size of the item.
    static constexpr size_t text_size = traits_type::text_size;
    /// true if this item is clickable, defaults to false.
    // static constexpr bool clickable = sgl::get_clickable_v<traits_type>;
    /// string_view type used by this item.
    using StringView = sgl::string_view<char_type>;
    /// string type used by this item for the text field.
    using String = sgl::static_string<char_type, text_size>;
    /// concrete input handler type.
    using InputHandler_t = sgl::Callable<sgl::error(item_type&, sgl::Input)>;
    /// concrete tick handler type.
    using TickHandler_t = sgl::Callable<void(item_type&)>;

    /// \brief Construct an item with text.
    /// \param text text of the item
    explicit constexpr ItemBase(StringView text) noexcept;

    /// \brief Construct an item with name, text and input handler.
    /// \tparam InputHandler input handler type
    /// \param text text of the item
    /// \param handler input handler
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, ItemImpl> = true>
    constexpr ItemBase(StringView text, InputHandler&& handler) noexcept;

    /// \brief Construct an item with name, text and tick handler. The tick handler is called every
    /// time the menu's tick() method is called.
    /// \tparam TickHandler tick handler type
    /// \param text text of the item
    /// \param handler tick handler
    template <typename TickHandler, enable_if_is_tick_handler<TickHandler, ItemImpl> = true>
    constexpr ItemBase(StringView text, TickHandler&& handler) noexcept;

    /// \brief construct an item with text and custom input and tick handlers. See InputHandler_t
    /// and TickHandler_t for more details.
    ///
    /// \tparam InputHandler input handler type
    /// \tparam TickHandler tick handler type
    /// \param text text of the item
    /// \param input_handler custom input handler
    /// \param tick_handler custom tick handler
    template <typename InputHandler,
              typename TickHandler,
              enable_if_is_input_handler<InputHandler, item_type> = true,
              enable_if_is_tick_handler<TickHandler, item_type> = true>
    constexpr ItemBase(StringView     text,
                       InputHandler&& input_handler,
                       TickHandler&&  tick_handler) noexcept;

    /// \brief Can be used to give access to the menu from an item. sgl::Menu will
    /// call this function for all items it contains with its declared type, i.e.
    /// this function can be statically overridden in derived classes.See
    /// PageLink for an example of how it is used.
    /// \tparam Menu sgl::Menu type
    /// \param m pointer to menu
    template <typename Menu>
    constexpr void set_menu(Menu* m) noexcept;

    /// \brief calls the items input handler. Usually called by the containing page.
    /// \param input input to handle.
    /// \return sgl::error::no_error in case of no error.
    /// \return sgl::error::edit_finished in case the item is done being edited.
    /// See input handling for more details.
    constexpr sgl::error handle_input(sgl::Input input) noexcept;

    /// invoke tick handler. Usually called by the page that owns this item.
    constexpr void tick() noexcept;

    /// \brief set the text field
    /// \param new_text new text
    /// \return sgl::error
    constexpr sgl::error set_text(StringView new_text) noexcept;

    /// \brief get mutable access to text field
    /// \return String&
    constexpr String& text() noexcept { return text_; }

    /// \brief get const reference to text field
    /// \return const String&
    constexpr const String& text() const noexcept { return text_; }

    /// \brief clear the text field.
    constexpr void clear_text() noexcept;

    /// \brief Set the input handler.
    /// \tparam InputHandler input handler type
    /// \param handler custom [input handler](concepts.md#tick-handler).
    /// \return item_type&
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr item_type& set_input_handler(InputHandler&& handler) noexcept;

    /// \brief Set the tick handler of this item.
    /// \tparam TickHandler tick handler type
    /// \param handler custom [tick handler](concepts.md#tick-handler).
    /// \return item_type&
    template <typename TickHandler, enable_if_is_tick_handler<TickHandler, item_type> = true>
    constexpr item_type& set_tick_handler(TickHandler&& handler) noexcept;

  private:
    /// default input handler. simply returns sgl::error::edit_finished.
    /// \return sgl::error
    static sgl::error default_handle_input(item_type&, sgl::Input) noexcept;

    InputHandler_t handler_{&default_handle_input}; ///< handles user input
    TickHandler_t  tick_handler_{};                 ///< handles tick update
    String         text_{};                         ///< text field
  };
} // namespace sgl

#include "sgl/impl/item_base_impl.hpp"
#endif /* SGL_ITEM_BASE_HPP */
