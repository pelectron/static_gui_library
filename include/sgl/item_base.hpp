#ifndef SGL_ITEM_BASE_HPP
#define SGL_ITEM_BASE_HPP
#include "sgl/callable.hpp"
#include "sgl/item_traits.hpp"
#include "sgl/static_string.hpp"
#include "sgl/string_view.hpp"

namespace sgl {

  /// \brief CRTP base class for every item type.
  /// \details An item  minimally consists of a text field, an input handler and a tick handler. It
  /// is used to reduce code duplication and NOT for runtime polymorphism. The input and tick
  /// handler allow flexible behavioral changes without a need to create a new derived type for
  /// every differently acting item using the same old data.
  ///
  /// The text field contains the text which the item should show. It is a static_string, i.e. does
  /// not heap allocate.
  ///
  /// The input and tick handler are also non heap allocating, although capturing lambda's and
  /// similar custom function objects can only be bound via placement new. See \ref sgl::Callable<Ret(Args...)> "Callable" for
  /// more details.
  ///
  /// \section item_input_handling Item Input Handling
  /// The input handler handles user input. It get's called every time the item's handle_input()
  /// method is called. The signature of a input handler is ``sgl::error(item_type&, sgl::Input) noexcept``.
  /// The item's input handler is called when a page is in edit mode and the
  /// page receives an input. The input handler's job is to decide what to do with the given user
  /// input, and return the appropiate sgl::error value. The library default way of handling things
  /// is the following:
  ///  - if the item is clickable (as indicated by the item traits), the handler will call the
  ///  item's click() method and return sgl::error::edit_finished if the return value of click() is
  ///  sgl::error::no_error. This achieves the effect of only needing to click an item once to
  ///  get an effect and not be stuck editing the item. If the click() call returns something other
  ///  than sgl::error::no_error, the call's return value will be returned.
  ///  - if the item is not clickable, sgl::error::edit_finished is returned, i.e. by default items
  ///  cannot be edited.
  ///  - if an input handler is provided by the user, it is simply invoked and the behaviour is
  ///  completely up to the provided handler.
  ///
  /// Your custom input handler should keep to the conventions of the input handler return value,
  /// i.e. return sgl::error::no_error to keep the page edit mode, sgl::error::edit_finished to make
  /// the page go into navigation mode and other values to indicate failures and also switch to edit
  /// mode.
  ///
  /// \section item_tick_handling Item Tick Handling
  /// The tick handler is used for items which need to be periodically updated externally (i.e. for
  /// displaying sensor values etc.). It gets called every time the item's tick() method is called.
  /// The signature of a tick handler is void(item_type&) noexcept. The tick handler is just passed
  /// a reference to the item it belongs to and does not return a value. The tick handlers of all
  /// items in a menu are then called at once through a call to the menu's tick() method. This makes
  /// the logic of how to update an item displaying a sensor value local to the item, while making
  /// the invocation as simple as possible. As the end user, you can simply call the menu's tick()
  /// on every gui loop or however often you see fit.
  ///
  ///
  /// \tparam ItemImpl concrete item type.
  /// \tparam Traits ItemTraits type for ItemImpl. This is used because accessing inner typedefs of
  /// yet undefined classes cannot be done in C++.
  template <typename ItemImpl, typename Traits = sgl::ItemTraits<ItemImpl>>
  class ItemBase {
  public:
    static_assert(sgl::has_item_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'item_type'.");
    static_assert(sgl::has_char_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'char_type'.");
    static_assert(sgl::has_text_size_v<Traits>,
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
    static constexpr bool clickable = sgl::get_clickable_v<traits_type>;
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
    /// \param input input to handle
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
    /// \param handler input handler
    /// \return item_type&
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    constexpr item_type& set_input_handler(InputHandler&& handler) noexcept;

    /// \brief Set the tick handler of this item
    /// \tparam TickHandler tick handler type
    /// \param handler custom tick handler
    /// \return item_type&
    template <typename TickHandler, enable_if_is_tick_handler<TickHandler, item_type> = true>
    constexpr item_type& set_tick_handler(TickHandler&& handler) noexcept;

  private:
    /// default input handler. simply returns sgl::error::edit_finished.
    /// \return sgl::error
    static sgl::error default_handle_input(item_type&, sgl::Input) noexcept;

    InputHandler_t handler_{&default_handle_input}; ///< handles user input
    TickHandler_t  tick_handler_{};                 ///< handles external update
    String         text_{};                         ///< text field
  };
} // namespace sgl
#include "sgl/impl/item_base_impl.hpp"
#endif