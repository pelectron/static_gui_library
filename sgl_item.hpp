#ifndef SGL_ITEM_HPP
#define SGL_ITEM_HPP
#include "sgl_callable.hpp"
#include "sgl_error.hpp"
#include "sgl_input.hpp"
#include "sgl_static_string.hpp"

#include <type_traits>

namespace sgl {
  /**
   * @brief Base class of all items.
   * It has a name, a text field, and an input handler. The name can be set only
   * once during the lifetime of an item and can be accessed with the get_name()
   * function.
   * The text field is a StaticString with a capacity of LineWidth. Access it
   * with the get_text() functions.
   * 
   * The input handlers concrete type is the alias InputHandler_t, which is a
   * sgl::Callable with the signature sgl::error(Item_t<LineWidth, CharT>&,
   * sgl::Input).
   * @tparam LineWidth display width
   * @tparam CharT character type
   */
  template <size_t LineWidth, typename CharT>
  class Item_t {
  public:
    /// string_view type used by this item.
    using StringView = sgl::string_view<CharT>;

    /// text field type of this item.
    using TextField_t = StaticString<CharT, LineWidth>;

    /// concrete input handler type.
    using InputHandler_t =
        sgl::Callable<sgl::error(Item_t<LineWidth, CharT>&, sgl::Input)>;

    /**
     * @brief SFINAE alias to check input handler types. A item input handler
     * must have a calling signature of:
     * sgl::error(Item_t<LineWidth, CharT>&,sgl::Input).
     * @tparam H handler type
     */
    template <typename H>
    using input_handler_check =
        std::enable_if_t<std::is_invocable_r_v<sgl::error,
                                               H,
                                               Item_t<LineWidth, CharT>&,
                                               sgl::Input>>;

    /**
     * @brief Construct an item with a name and text.
     * @param name name of the item
     * @param text text of the item
     */
    Item_t(StringView name, StringView text) : name_(name), text_(text) {}

    /**
     * @brief Construct an item with it's name and text set to name_and_text.
     * @param name_and_text name and text of item.
     */
    Item_t(StringView name_and_text)
        : name_(name_and_text), text_(name_and_text) {}

    /**
     * @brief Construct an item with name, text and input handler.
     * @tparam InputHandler input handler type
     * @param name name of the item
     * @param text text of the item
     * @param handler input handler
     */
    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Item_t(StringView name, StringView text, InputHandler&& handler)
        : handler(std::forward<InputHandler>(handler)), name_(name),
          text_(text) {}

    /**
     * @brief Construct an item with name, text and input handler.
     * @tparam InputHandler input handler type
     * @param name_and_text name and text of item
     * @param handler input handler
     */
    template <typename InputHandler,
              input_handler_check<InputHandler>* = nullptr>
    Item_t(StringView name_and_text, InputHandler&& handler)
        : handler(std::forward<InputHandler>(handler)),
          name_(name_and_text), text_{name_and_text} {}

    /**
     * @brief Can be used to give access to the menu from an item. Menu_t will
     * call this function for all items it contains with its declared type, i.e.
     * this function can be statically overridden in derived classes.See
     * PageLink_t for an example of how it is used.
     * @tparam Menu menu type
     */
    template <typename Menu>
    void set_menu(Menu*) {
      static_assert(sgl::is_menu_v<Menu>, "Menu type must be a menu");
    }

    /**
     * @brief calls the items input handler.
     * @param input input
     * @return sgl::error::no_error in case of no error.
     * @return sgl::error::edit_finished in case the item is done being edited.
     * See input handling for more details.
     */
    sgl::error handle_input(sgl::Input input) { return handler(*this, input); }

    /**
     * @brief set the text field
     * @param new_text new text
     * @return sgl::error
     */
    sgl::error set_text(StringView new_text) {
      text_ = new_text;
      return sgl::error::no_error;
    }

    /**
     * @brief get const reference to text field
     * @return const TextField_t&
     */
    const TextField_t& get_text() const { return text_; }

    /**
     * @brief get reference to text field
     * @return TextField_t&
     */
    TextField_t& get_text() { return text_; }

    /**
     * @brief get the name of the item
     * @return StringView
     */
    StringView get_name() const { return name_; }

    /**
     * @brief clear the text field.
     */
    void clear_text() { text_.reset(); }

    /**
     * @brief Set the input handler
     * @tparam InputHandler input handler type
     * @param handler input handler
     */
    template <typename InputHandler>
    void set_input_handler(InputHandler&& handler) {
      static_assert(std::is_invocable_r_v<sgl::error,
                                          InputHandler,
                                          Item_t<LineWidth, CharT>&,
                                          sgl::Input>,
                    "the provided handler is not a valid input handler");
      if constexpr (std::is_same_v<InputHandler, InputHandler_t>) {
        this->handler = handler;
      } else {
        this->handler.bind(std::forward<InputHandler>(handler));
      }
    }

  private:
    /// default input handler. Does nothing.
    static sgl::error default_handle_input(Item_t<LineWidth, CharT>&,
                                           sgl::Input) {
      return sgl::error::edit_finished;
    }

    InputHandler_t handler{&default_handle_input}; ///< input handler
    StringView     name_{};                        ///< name
    TextField_t    text_{};                        ///< text field
  };

} // namespace sgl
#endif