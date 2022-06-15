#ifndef SGL_BASIC_ITEM_HPP
#define SGL_BASIC_ITEM_HPP
#include "sgl_item_traits.hpp"
#include "sgl_callable.hpp"
#include "sgl_static_string.hpp"

namespace sgl {

  /**
   * @brief
   * @tparam Traits
   */
  template <typename Traits>
  class ItemBase {
  public:
    static_assert(sgl::has_item_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'item_type'.");
    static_assert(sgl::has_char_type_typedef_v<Traits>,
                  "Traits type needs an inner typename named 'char_type'.");
    static_assert(sgl::has_text_size_v<Traits>,
                  "Traits type needs a static constexpr member of type size_t called 'text_size'.");

    using traits_type = Traits;
    using item_type = typename Traits::item_type;
    using char_type = typename Traits::char_type;
    static constexpr size_t text_size = Traits::text_size;
    static constexpr bool   clickable = sgl::get_clickable_v<Traits>;
    using StringView = sgl::string_view<char_type>;
    using String = sgl::static_string<char_type, text_size>;

    /// concrete input handler type.
    using InputHandler_t = sgl::Callable<sgl::error(item_type&, sgl::Input)>;

    /**
     * @brief Construct an item with a name and text.
     * @param name name of the item
     * @param text text of the item
     */
    constexpr ItemBase(StringView name, StringView text) noexcept;

    /**
     * @brief Construct an item with it's name and text set to name_and_text.
     * @param name_and_text name and text of item.
     */
    constexpr ItemBase(StringView name_and_text) noexcept;

    /**
     * @brief Construct an item with name, text and input handler.
     * @param name name of the item
     * @param text text of the item
     * @param handler input handler
     */
    constexpr ItemBase(StringView name,
                       StringView text,
                       sgl::error (*handler)(item_type&, sgl::Input) noexcept) noexcept;

    /**
     * @brief Construct an item with name, text and input handler.
     * @tparam InputHandler input handler type
     * @param name name of the item
     * @param text text of the item
     * @param handler input handler
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    ItemBase(StringView name, StringView text, InputHandler&& handler) noexcept;

    /**
     * @brief Construct an item with name, text and input handler.
     * @param name_and_text name and text of item
     * @param handler input handler
     */
    constexpr ItemBase(StringView name_and_text,
                       sgl::error (*handler)(item_type&, sgl::Input) noexcept) noexcept;

    /**
     * @brief Construct an item with name, text and input handler.
     * @tparam InputHandler input handler type
     * @param name_and_text name and text of item
     * @param handler input handler
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    ItemBase(StringView name_and_text, InputHandler&& handler) noexcept;

    /**
     * @brief Can be used to give access to the menu from an item. Menu_t will
     * call this function for all items it contains with its declared type, i.e.
     * this function can be statically overridden in derived classes.See
     * basic_page_link for an example of how it is used.
     * @tparam Menu menu type
     */
    template <typename Menu>
    constexpr void set_menu(Menu*) noexcept;

    /**
     * @brief calls the items input handler.
     * @param input input
     * @return sgl::error::no_error in case of no error.
     * @return sgl::error::edit_finished in case the item is done being edited.
     * See input handling for more details.
     */
    constexpr sgl::error handle_input(sgl::Input input) noexcept;

    /**
     * @brief set the text field
     * @param new_text new text
     * @return sgl::error
     */
    constexpr sgl::error set_text(StringView new_text) noexcept;

    /**
     * @brief get const reference to text field
     * @return const String&
     */
    constexpr const String& get_text() const noexcept;

    /**
     * @brief get reference to text field
     * @return String&
     */
    constexpr String&    get_text() noexcept;
    constexpr StringView text() const noexcept { return text_; }
    /**
     * @brief clear the text field.
     */
    constexpr void clear_text() noexcept;

    /**
     * @brief get the name of the item
     * @return StringView
     */
    constexpr StringView name() const noexcept;

    /**
     * @brief Set the input handler.
     * @tparam InputHandler input handler type
     * @param handler input handler
     */
    template <typename InputHandler, enable_if_is_input_handler<InputHandler, item_type> = true>
    void set_input_handler(InputHandler&& handler) noexcept;

  private:
    /// default input handler. Does nothing.
    static sgl::error default_handle_input(item_type& item, sgl::Input) noexcept;

    InputHandler_t handler_{&default_handle_input}; ///< input handler
    StringView     name_{};                         ///< name
    String         text_{};                         ///< text field
  };

  template <typename Traits>
  constexpr ItemBase<Traits>::ItemBase(StringView name, StringView text) noexcept
      : name_(name), text_(text) {}

  template <typename Traits>
  constexpr ItemBase<Traits>::ItemBase(StringView name_and_text) noexcept
      : name_(name_and_text), text_(name_and_text) {}

  template <typename Traits>
  constexpr ItemBase<Traits>::ItemBase(StringView name_and_text,
                                       sgl::error (*handler)(item_type&,
                                                             sgl::Input) noexcept) noexcept
      : handler_(handler), name_(name_and_text), text_(name_and_text) {}

  template <typename Traits>
  constexpr ItemBase<Traits>::ItemBase(StringView name,
                                       StringView text,
                                       sgl::error (*handler)(item_type&,
                                                             sgl::Input) noexcept) noexcept
      : handler_(handler), name_(name), text_(text) {}

  template <typename Traits>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, typename Traits::item_type>>
  ItemBase<Traits>::ItemBase(StringView name_and_text, InputHandler&& handler) noexcept
      : handler_(forward<InputHandler>(handler)), name_(name_and_text), text_{name_and_text} {}

  template <typename Traits>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, typename Traits::item_type>>
  ItemBase<Traits>::ItemBase(StringView name, StringView text, InputHandler&& handler) noexcept
      : handler_(forward<InputHandler>(handler)), name_(name), text_(text) {}

  template <typename Traits>
  template <typename Menu>
  constexpr void ItemBase<Traits>::set_menu(Menu*) noexcept {}

  template <typename Traits>
  constexpr sgl::error ItemBase<Traits>::handle_input(sgl::Input input) noexcept {
    return handler_(*static_cast<item_type*>(this), input);
  }

  template <typename Traits>
  constexpr sgl::error ItemBase<Traits>::set_text(StringView new_text) noexcept {
    text_ = new_text;
    return sgl::error::no_error;
  }

  template <typename Traits>
  constexpr const typename ItemBase<Traits>::String& ItemBase<Traits>::get_text() const noexcept {
    return text_;
  }

  template <typename Traits>
  constexpr typename ItemBase<Traits>::String& ItemBase<Traits>::get_text() noexcept {
    return text_;
  }

  template <typename Traits>
  constexpr typename ItemBase<Traits>::StringView ItemBase<Traits>::name() const noexcept {
    return name_;
  }

  template <typename Traits>
  constexpr void ItemBase<Traits>::clear_text() noexcept {
    text_.reset();
  }

  template <typename Traits>
  template <typename InputHandler,
            enable_if_is_input_handler<InputHandler, typename Traits::item_type>>
  void ItemBase<Traits>::set_input_handler(InputHandler&& handler) noexcept {
    static_assert(is_invocable_r_v<sgl::error, InputHandler, item_type&, sgl::Input>,
                  "the provided handler is not a valid input handler.");
    if constexpr (is_same_v<InputHandler, InputHandler_t>) {
      handler_ = handler;
    } else {
      handler_.bind(forward<InputHandler>(handler));
    }
  }

  template <typename Traits>
  sgl::error ItemBase<Traits>::default_handle_input(item_type& item, sgl::Input) noexcept {
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
} // namespace sgl
#endif